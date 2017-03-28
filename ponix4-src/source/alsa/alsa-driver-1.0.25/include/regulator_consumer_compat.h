/* stripped version from 2.6.32 kernel */
#ifndef __LINUX_REGULATOR_CONSUMER_H_
#define __LINUX_REGULATOR_CONSUMER_H_

#define REGULATOR_MODE_FAST			0x1
#define REGULATOR_MODE_NORMAL			0x2
#define REGULATOR_MODE_IDLE			0x4
#define REGULATOR_MODE_STANDBY			0x8

#define REGULATOR_EVENT_UNDER_VOLTAGE		0x01
#define REGULATOR_EVENT_OVER_CURRENT		0x02
#define REGULATOR_EVENT_REGULATION_OUT		0x04
#define REGULATOR_EVENT_FAIL			0x08
#define REGULATOR_EVENT_OVER_TEMP		0x10
#define REGULATOR_EVENT_FORCE_DISABLE		0x20
#define REGULATOR_EVENT_VOLTAGE_CHANGE		0x40

struct regulator;

struct regulator_bulk_data {
	const char *supply;
	struct regulator *consumer;
};

static inline struct regulator *regulator_get(struct device *dev,
	const char *id)
{
	return (struct regulator *)id;
}
static inline void regulator_put(struct regulator *regulator)
{
}

static inline int regulator_enable(struct regulator *regulator)
{
	return 0;
}

static inline int regulator_disable(struct regulator *regulator)
{
	return 0;
}

static inline int regulator_is_enabled(struct regulator *regulator)
{
	return 1;
}

static inline int regulator_bulk_get(struct device *dev,
				     int num_consumers,
				     struct regulator_bulk_data *consumers)
{
	return 0;
}

static inline int regulator_bulk_enable(int num_consumers,
					struct regulator_bulk_data *consumers)
{
	return 0;
}

static inline int regulator_bulk_disable(int num_consumers,
					 struct regulator_bulk_data *consumers)
{
	return 0;
}

static inline void regulator_bulk_free(int num_consumers,
				       struct regulator_bulk_data *consumers)
{
}

static inline int regulator_set_voltage(struct regulator *regulator,
					int min_uV, int max_uV)
{
	return 0;
}

static inline int regulator_get_voltage(struct regulator *regulator)
{
	return 0;
}

static inline int regulator_set_current_limit(struct regulator *regulator,
					     int min_uA, int max_uA)
{
	return 0;
}

static inline int regulator_get_current_limit(struct regulator *regulator)
{
	return 0;
}

static inline int regulator_set_mode(struct regulator *regulator,
	unsigned int mode)
{
	return 0;
}

static inline unsigned int regulator_get_mode(struct regulator *regulator)
{
	return REGULATOR_MODE_NORMAL;
}

static inline int regulator_set_optimum_mode(struct regulator *regulator,
					int load_uA)
{
	return REGULATOR_MODE_NORMAL;
}

static inline int regulator_register_notifier(struct regulator *regulator,
			      struct notifier_block *nb)
{
	return 0;
}

static inline int regulator_unregister_notifier(struct regulator *regulator,
				struct notifier_block *nb)
{
	return 0;
}

static inline void *regulator_get_drvdata(struct regulator *regulator)
{
	return NULL;
}

static inline void regulator_set_drvdata(struct regulator *regulator,
	void *data)
{
}

#endif
