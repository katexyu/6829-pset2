#include <iostream>
#include <set>
#include <algorithm>

#include "controller.hh"
#include "timestamp.hh"
#include "parameters.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_ { debug },
    window_size_ { 1 },
    rtts_ { },
    epoch_start_ { timestamp_ms() },
    previous_error_ { 0 }
{}

void Controller::maybe_advance_epoch()
{
  uint64_t now = timestamp_ms();
  if (now - this->epoch_start_ < CONTROL_EPOCH) {
    return;
  }

  // what was 95th percentile?
  vector<uint64_t> flattened;
  double decay_multiplier = 1.0;
  for (int i = this->rtts_.size() - 1; i >= 0; i--) {
    for (auto rtt : this->rtts_[i]) {
      // exponentially weight older history
      // so we don't overreact to bad things
      flattened.push_back(rtt * decay_multiplier);
    }
    decay_multiplier *= HISTORY_DECAY;
  }
  sort(flattened.begin(), flattened.end());
  size_t count = flattened.size();
  if (count == 0) {
    return; // nothing to do
  }
  size_t index = count * 0.95;
  auto actual = flattened[index];

  // control decisions
  /*
  if (actual > TARGET_95_PERCENTILE) {
    // too laggy, slow down
    this->window_size_ /= MD_FACTOR;
  } else {
    // can afford to be slower
    this->window_size_ += AI_FACTOR;
  }
  */

  double error = TARGET_95_PERCENTILE - actual;
  double d_error = error - this->previous_error_;
  this->previous_error_ = error;
  double output = K_P * error + K_D * d_error;
  this->window_size_ += output;

  // debugging info
  if (debug_) {
    auto sum = std::accumulate(flattened.begin(), flattened.end(), 0);
    auto avg = sum / flattened.size();
    cerr << "Epoch end, # packets = " << flattened.size()
      << " avg rtt = " << avg << " 95th percentile = " << actual
      << " window = " << this->window_size_
      << endl;
  }

  // next epoch
  this->epoch_start_ = now;
  if (this->rtts_.size() >= HISTORY_SIZE) {
    this->rtts_.erase(this->rtts_.begin());
  }
  this->rtts_.push_back({}); // cycle
}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  maybe_advance_epoch();

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
  maybe_advance_epoch();
  if (this->rtts_.size() == 0) {
    this->rtts_.push_back({});
  }
  uint64_t rtt = timestamp_ack_received - send_timestamp_acked;
  this->rtts_[this->rtts_.size() - 1].push_back(rtt);

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
