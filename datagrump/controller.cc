#include <iostream>
#include <set>

#include "controller.hh"
#include "timestamp.hh"
#include "parameters.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_ { debug },
    window_size_ { 1 },
    rtt_estimate_ { 0 },
    integral_error_ { 0 },
    previous_error_ { 0 }
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
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
  // update rtt estimate
  auto rtt = timestamp_ack_received - send_timestamp_acked;
  if (this->rtt_estimate_ == 0) {
    this->rtt_estimate_ = rtt;
  } else {
    this->rtt_estimate_ = RTT_EWMA_FACTOR * rtt + (1 - RTT_EWMA_FACTOR) * this->rtt_estimate_;
  }

  double error = TARGET_DELAY - rtt;
  this->integral_error_ += error;
  double derivative_error = error - this->previous_error_;
  this->previous_error_ = error;

  double k_d = (derivative_error < 0) ? K_D_neg : K_D_pos;
  double output = K_P * error + K_I * this->integral_error_ + k_d * derivative_error;
  if (output < 0 && error < 0) {
    this->integral_error_ -= error; // subtract it back out
    // prevent "reset windup"
  }
  this->window_size_ = output;

  if ( debug_ ) {
    cerr << "Target: " << TARGET_DELAY << " rtt: " << rtt << endl;
    cerr << "At time " << timestamp_ack_received
      << " received ack for datagram " << sequence_number_acked << endl;
    cerr << "  error = " << error << " window size set to: " << output << endl;
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
