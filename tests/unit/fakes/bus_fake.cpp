
#include "bus_fake.h"
#include <algorithm>
#include <errno.h>
#include <cstring>

MuBusFake* MuBusFakeObj = nullptr;

static int init()
{
	return MuBusFakeObj->init();
}

static int publish(const struct zbus_channel *chan, const void* msg)
{
	return MuBusFakeObj->publish(chan, msg);
}

static int addObserver(const struct zbus_channel *chan, const struct zbus_observer* obs, struct zbus_observer_node* node)
{
	return MuBusFakeObj->addObserver(chan, obs, node);
}

static int removeObserver(const struct zbus_channel *chan, const struct zbus_observer *obs)
{
	return MuBusFakeObj->removeObserver(chan, obs);
}

static int read(const struct zbus_channel *chan, void* msg)
{
	return MuBusFakeObj->read(chan, msg);
}

static const void* constMessage(const struct zbus_channel *chan)
{
	return MuBusFakeObj->constMessage(chan);
}

MuBusFake::MuBusFake()
{

}

int MuBusFake::init()
{
	return 0;
}

int MuBusFake::publish(const struct zbus_channel *chan, const void* msg)
{
	if (!chan || !msg) {
		return -EINVAL;
	}

	memcpy(chan->message_buffer, msg, chan->message_size);

	// Trigger only observers currently linked to this channel
	for (auto node : mObservers) {
		if (node->chan == chan && node->obs->callback) {
			node->obs->callback(node->obs);
		}
	}
	return 0;
}

int MuBusFake::addObserver(const struct zbus_channel *chan, const struct zbus_observer* obs,
	struct zbus_observer_node* node)
{
	if (!chan || !obs || !node) {
		return -EINVAL;
	}

	node->chan = chan;
	node->obs = obs;
	mObservers.push_back(node);
	return 0;
}

int MuBusFake::removeObserver(const struct zbus_channel *chan, const struct zbus_observer *obs)
{
	if (!chan || !obs) {
		return -EINVAL;
	}

	mObservers.erase(std::remove_if(mObservers.begin(), mObservers.end(),
		[&](struct zbus_observer_node* n){ return n->chan == chan && n->obs == obs; }),
		mObservers.end());

	return 0;
}

int MuBusFake::read(const struct zbus_channel *chan, void* msg)
{
	memcpy(msg, chan->message_buffer, chan->message_size);
	return 0;
}


const void* MuBusFake::constMessage(const struct zbus_channel *chan)
{
	return chan->message_buffer;
}

struct mu_bus_if muBusFake = {
	.init = init,
	.publish = publish,
	.addObserver = addObserver,
	.removeObserver = removeObserver,
	.read = read,
	.constMessage = constMessage
};
