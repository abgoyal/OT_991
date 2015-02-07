

#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/cpuidle.h>

#include "cpuidle.h"

LIST_HEAD(cpuidle_governors);
struct cpuidle_governor *cpuidle_curr_governor;

static struct cpuidle_governor * __cpuidle_find_governor(const char *str)
{
	struct cpuidle_governor *gov;

	list_for_each_entry(gov, &cpuidle_governors, governor_list)
		if (!strnicmp(str, gov->name, CPUIDLE_NAME_LEN))
			return gov;

	return NULL;
}

int cpuidle_switch_governor(struct cpuidle_governor *gov)
{
	struct cpuidle_device *dev;

	if (gov == cpuidle_curr_governor)
		return 0;

	cpuidle_uninstall_idle_handler();

	if (cpuidle_curr_governor) {
		list_for_each_entry(dev, &cpuidle_detected_devices, device_list)
			cpuidle_disable_device(dev);
		module_put(cpuidle_curr_governor->owner);
	}

	cpuidle_curr_governor = gov;

	if (gov) {
		if (!try_module_get(cpuidle_curr_governor->owner))
			return -EINVAL;
		list_for_each_entry(dev, &cpuidle_detected_devices, device_list)
			cpuidle_enable_device(dev);
		cpuidle_install_idle_handler();
		printk(KERN_INFO "cpuidle: using governor %s\n", gov->name);
	}

	return 0;
}

int cpuidle_register_governor(struct cpuidle_governor *gov)
{
	int ret = -EEXIST;

	if (!gov || !gov->select)
		return -EINVAL;

	mutex_lock(&cpuidle_lock);
	if (__cpuidle_find_governor(gov->name) == NULL) {
		ret = 0;
		list_add_tail(&gov->governor_list, &cpuidle_governors);
		if (!cpuidle_curr_governor ||
		    cpuidle_curr_governor->rating < gov->rating)
			cpuidle_switch_governor(gov);
	}
	mutex_unlock(&cpuidle_lock);

	return ret;
}

static struct cpuidle_governor *cpuidle_replace_governor(int exclude_rating)
{
	struct cpuidle_governor *gov;
	struct cpuidle_governor *ret_gov = NULL;
	unsigned int max_rating = 0;

	list_for_each_entry(gov, &cpuidle_governors, governor_list) {
		if (gov->rating == exclude_rating)
			continue;
		if (gov->rating > max_rating) {
			max_rating = gov->rating;
			ret_gov = gov;
		}
	}

	return ret_gov;
}

void cpuidle_unregister_governor(struct cpuidle_governor *gov)
{
	if (!gov)
		return;

	mutex_lock(&cpuidle_lock);
	if (gov == cpuidle_curr_governor) {
		struct cpuidle_governor *new_gov;
		new_gov = cpuidle_replace_governor(gov->rating);
		cpuidle_switch_governor(new_gov);
	}
	list_del(&gov->governor_list);
	mutex_unlock(&cpuidle_lock);
}

