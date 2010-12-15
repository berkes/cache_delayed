# Delayed Cache module

Provided by [Dutch Open Projects](http://dop.nu)
Developed by [Bèr Kessels](http://webschuur.com)

## About
A simple module that allows certain `cache\_clear\_all()` calls to be delayed. 
Requires core hacks. only for certain 
For experienced Drupaleers only :) (it has severe side-effects, such as a much better working cache, or a broken site).

Cases: 
 * A site with commenting only for authenticated users will wipe cache for anonymous users everytime a comment is saved. 
 * A site where you want to enforce cache for a longer period, despite you (or other authenticated users) writing articles.
 * Any other site where you have problems building up a proper cache for blocks, pages and such, because it gets flushed by several (core) actions.

### How it works:###
Everytime a cache\_clear\_all is invoked, instead of clearing the cache, it will 
add an entry to a worker queue. 
When calling a (secured) url, we walk trough the queue, select those entries "old enough" and whipe them from your cache (and remove them from the queue).

### Why it was made ### 
For a certain site, we saw over 1000 queries to build pages. One quickfix was to use the cache better. Drupals caching system however, is <del>broken</del>not very smart. And will flush on every comment posted, node created, poll-voted and so on. We wanted to avoid these wipes, by enforcing the cache to remain unflushed for a little longer. Even if Drupal is stubborn and wipes it anyway.

The other option was to build the site in a [proper](http://rubyonrails.org) [framework](http://django.org) :)
And the last option, is to simply fix that rediculous amount of queries. But that is the long route.

## Core hacks
You have two options. 
1. Hack the actual cache\_clear\_all in core, so that /all/ cache\_clear\_alls go via the queue. This will have many side-effects. So know what you are doing. 
1. Hack certain places where cache\_clear\_all is called, for example comment_save(), to avoid a full cache-wipe only in that place. Has little side-effects. 

## Todo
Cherry-picking: Allow url-parameters to wipe cache with a cid, in one table or with a wildcard only. 

## Features?
Few. We opt for speed, not flexibility.

## Gotcha's
We insert, delete and update with delayed and low_priority. Thus only works on databases that support that. 
And the resulting status can most often not be used at all. After all: mysql may wait with inserting a broken or duplicate record, but never report this brokenness or duplicateness back to our code. Keep an eye on the logs. 
Use the Logs Luke. Use the Logs.
