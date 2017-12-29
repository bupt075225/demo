#ifndef __LIST_H__
#define __LIST_H__

struct list_hook {
    struct list_hook *prev;
    struct list_hook *next; 
};

/* Stolen list stuff from Linux kernel */

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/* Initialize a list head variable */
#define LIST_HEAD_INIT(name) { &(name), &(name) }
/* Define a list head variable and initialize it */
#define LIST_HEAD(name) \
	struct list_hook name = LIST_HEAD_INIT(name)

/* Initialize a list head pointer */
static inline void INIT_LIST_HEAD(struct list_hook *list)
{
    list->prev = list;
    list->next = list;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_hook *head)
{
	return head->prev == head;
}

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr); \
	(type *)( (char *)__mptr - offsetof(type,member) );})

/**
 * list_entry - get the struct for this entry
 * @ptr:     the &struct list_hook pointer.
 * @type:    the type of the struct this list hook is embedded in.
 * @member:  the name of the list hook within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_for_each_entry - iterate over list of given type
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list hook within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type
 * @pos: the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list hook within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)				\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))


#define list_del(elem) remque(elem)

#define list_del_init(elem) do {		\
		remque(elem);			\
		INIT_LIST_HEAD(elem);		\
	} while (0)

/**
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_hook *new,
                   struct list_hook *prev,
                   struct list_hook *nexe)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added.
 * @head: list head to add it after.
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_hook *new, struct list_hook *head)
{
    __list_add(new, head, head->next); 
}

/**
 *  list_add_tail - add a new entry
 *  @new: new entry to be added.
 *  @head: list head to add it before.
 *
 *  Insert a new entry before the specified head.
 *  This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_hook *new, struct list_hook *head)
{
    __list_add(new, head->prev, head);
}

/**
 * Delete a list entry by making the prev/next entries
 * points to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_hook *prev, sturct list_hook *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - delete entry from list.
 * @entry: the element to delete from the list.
 */
static inline void list_del(struct list_hook *entry)
{
    __list_del(entry->prev, entry->next);
    entry->prev = NULL;
    entry->next = NULL;
}

/**
 * list_replace - replace old entry by new one
 * @old: the element to be replaced
 * @new: the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_hook *old, 
                struct list_hook *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline void list_is_singular(struct list_hook *head)
{
    return !list_empty(head) && (head->next == head->prev);
}

static inline void __list_cut_position(struct list_hook *list,
        struct list_hook *head, struct list_hook *entry)
{
    struct list_hook *new_first = entry->next;
    list->next = head->next;
    list->next->prev = list;
    list->prev = entry;
    entry->next = list;
    head->next = new_first;
    new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries.
 * @head: a list with entries.
 * @entry: an entry within head, could be the head itself
 *  and if so we won't cut the list
 *
 *  This helper moves the initial part of @head, up to and
 *  including @entry, from @head to @list. You should pass
 *  on @entry an element you know is on @head. @list should 
 *  be an empty list or a list you do not care about losing
 *  its data.
 */
static inline void list_cut_position(struct list_hook *list,
         struct list_hook *head, struct list_hook *entry)
{
    if (list_empty(head))
        return;
    if (list_is_singular(head) &&
        (head->next != entry && head !=entry))
        return;
    if (entry==head)
        INIT_LIST_HEAD(list);
    else
        __list_cut_position(list, head, entry);
}

static inline void __list_splice(const struct list_hook *list,
                struct list_hook *prev,
                struct list_hook *next)
{
    struct list_hook *first = list->next;
    struct list_hook *last = list->prev;
    
    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * |---------------------------------------|
 * |   --------     ---------   --------   |
 * --- | list |<--->| first |...| last | <--  
 *     --------     ---------   --------
 *                 |                  |
 *                 |                  |
 *                 |                  |
 *        -------- |                  |---------   --------   
 *        | head |-------------------->| first |...| last |  
 *        --------                     ---------   --------
 *
 */
static inline void list_splice(struct list_hook *list,
                struct list_hook *head)
{
    if (!list_empty(list))
        __list_splice(list, head, head->next);
}

#endif /* __LIST_H__ */
