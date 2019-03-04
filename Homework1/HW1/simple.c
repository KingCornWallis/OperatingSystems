//Jacob Darabaris
//CS 470 P1
//1/24/18

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>

//birthday struct definition
typedef struct birthday
{
	int day;
	int month;
	int year;
	struct list_head list;
}birthday;

//delcare birthday_list of type LIST_HEAD
static LIST_HEAD(birthday_list);

/* This function is called when the module is loaded. */
int simple_init(void)
{
	printk(KERN_INFO "Loading Module\n");
	birthday *person;
	//initial declaration not allowed?
	int i = 0;
	//print 5 birthdays
	for(i = 0; i < 5; i++)
	{
		//allocate memory
		person = kmalloc(sizeof(*person), GFP_KERNEL);
		//adding i so the ouput is not identical
		person->day = 2;
		person->month = 8;
		person->year = 1995;
		INIT_LIST_HEAD(&person->list);
		//appends to the end of the list
		list_add_tail(&person->list, &birthday_list);
	}
	
	birthday *ptr;
	list_for_each_entry(ptr, &birthday_list, list)
	{
		printk(KERN_INFO "Birthday: %d/%d/%d\n", 
			ptr->month, ptr->day, ptr->year);
	}

	return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) 
{
	printk(KERN_INFO "Removing Module\n");
	/*on each iteration ptr points to the next birthday struct */
	birthday *ptr;
	birthday *next;
	list_for_each_entry_safe(ptr, next, &birthday_list, list)
	{	
		printk(KERN_INFO "Remove %d/%d/%d\n", 
			ptr->month, ptr->day, ptr->year);
		//clean up
		list_del(&ptr->list);
		kfree(ptr);
	}
	printk(KERN_INFO "Task complete.\n");
}


/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");



