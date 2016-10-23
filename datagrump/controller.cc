#include <iostream>
#include <set>

#include "controller.hh"
#include "timestamp.hh"

#define AI_FACTOR (1.0)
#define MD_FACTOR (2.0)
#define RTT_EWMA_FACTOR (0.2)
#define LATE_FACTOR (1.2)

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_ { debug },
    window_size_ { 1 },
    pending_ { },
    rtt_estimate_ { 0 }
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  // do multiplicative decrease
  std::set<uint64_t> to_remove;
  auto now = timestamp_ms();
  for (auto const &p : this->pending_) {
    auto seq = p.first;
    auto ts = p.second;
    if (this->rtt_estimate_ != 0 && now - ts > this->rtt_estimate_ * LATE_FACTOR) {
      to_remove.insert(seq);
      this->window_size_ /= MD_FACTOR;
    }
  }
  for (auto const &s : to_remove) {
    this->pending_.erase(s);
  }

  // ensure window size is at least 1
  if (this->window_size_ < 1) {
    this->window_size_ = 1;
  }

  unsigned int rounded = this->window_size_;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << rounded << endl;
  }

  return rounded;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  this->pending_[sequence_number] = send_timestamp;

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  // additive increase
  // increase by 1/cwnd for each ack
  this->window_size_ += AI_FACTOR / (this->window_size_);

  // remove from pending
  this->pending_.erase(sequence_number_acked);

  // update rtt estimate
  auto rtt = timestamp_ack_received - send_timestamp_acked;
  if (this->rtt_estimate_ == 0) {
    this->rtt_estimate_ = rtt;
  } else {
    this->rtt_estimate_ = RTT_EWMA_FACTOR * rtt + (1 - RTT_EWMA_FACTOR) * this->rtt_estimate_;
  }

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}
