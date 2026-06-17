#pragma once

#include "client.h"
#include "flwr/proto/message.pb.h"

flwr::proto::Message handle_message(flwr_local::Client *client,
                                    const flwr::proto::Message &message);
