<?php
/**
 * Module:      cache_delayed
 * Date:        2010-12-15  14:45
 * Author:      ber
 *
 * Description:
 *   Delays cache_clear_all
 * @file include for cache_delayed mechanism. The module is not always available, 
 *       hence the include.
 *
 * License:
 *   
 *   Copyright (C) 2010  ber
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.pro See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
/**
 * Wrapper around queue function
 * Drop-in replacement for cache_clear_all()
 *
 * Example usage:
 * @code
 *  # HACK: replaced the cache_clear_all with our own cache_delayed_clear_all
 *  # cache_clear_all()
 *  cache_delayed_clear_all();
 * @endcode
 * 
 * @param $cid
 *   Description of param $cid
 * @param $table
 *   Description of param $table
 * @param $wildcard
 *   Description of param $wildcard
 *
 * @return
 *   Description of return value
 */
function cache_delayed_clear_all($cid = NULL, $table = NULL, $wildcard = FALSE) {
  cache_delayed_queue($cid, $table, $wildcard);
}

/** Add an item to the queue.
 *
 * Checks if a similar item exists in quwueue, if so, updates, else inserts new item in queue.
 *
 * Example usage:
 * @code
 *  my_print('Function usage sample');
 * @endcode
 * 
 * @param $cid
 *   Description of param $cid
 * @param $table
 *   Description of param $table
 * @param $wildcard
 *   Description of param $wildcard
 *
 * @return
 *   Description of return value
 */
function cache_delayed_queue($cid = NULL, $table = NULL, $wildcard = FALSE) {
  $status = FALSE;
  # In case of a matching record, update instead.
  if ($record = _cache_delayed_db_read($cid, $table, $wildcard)) {
    $status = _cache_delayed_db_update($record['qid'], $record['cid'], $record['table'], $record['wildcard']);
  }
  else { #insert a new item.
    $status = _cache_delayed_db_create($cid, $table, $wildcard);
  }

  return $status;
}

/**
 * CRUD functions
 */ 
/** Inserts a new record into cache_delayed table.
 *
 * @param $cid String
 *   Cache-identifier parameter in cache_clear_all.
 * @param $table String
 *   Table parameter in cache_clear_all.
 * @param $wildcard String
 *   Wildcard parameter in cache_clear_all.
 *
 * @return
 *   Failure to write a record will return FALSE. Otherwise SAVED_NEW.
 */
function _cache_delayed_db_create($cid = '', $table = '' , $wildcard = '') {
  $created_at = $updated_at = time();
  //We do not use 
  $res = db_query(
    "INSERT DELAYED INTO {cache_delayed} 
    (cid, table, wildcard, created_at, updated_at) 
    VALUES
    ('%s', '%s', '%s', %d, %d) 
  ", $cid, $table, $wildcard, $created_at, $updated_at);
  return ($res ? SAVED_NEW : FALSE); 
}

/** Reads a single record from the database, that matches the provided parameters.
 *
 * @param $qid
 *   Description of param $qid
 * @param $cid
 *   Description of param $cid
 * @param $table
 *   Description of param $table
 * @param $wildcard
 *   Description of param $wildcard
 *
 * @return
 *   Description of return value
 */
function _cache_delayed_db_read($qid = NULL, $cid = '', $table = '', $wildcard = '') {
  $record = array();

  if ($qid) {
    $wheres[] = 'qid = %d';
    $params[] = $qid;
  }
  if ($cid) {
    $wheres[] = "cid = '%s'";
    $params[] = $cid;
  }
  if ($cid) {
    $wheres[] = "cid = '%s'";
    $params[] = $cid;
  }
  if ($table) {
    $wheres[] = "table = '%s'";
    $params[] = $table;
  }
  if ($wildcard) {
    $wheres[] = "wildcard = '%s'";
    $params[] = $wildcard;
  }

  if (count($wheres)) {
    $sql = 'SELECT * FROM {cache_delayed} WHERE '. join('AND ', $wheres) .' LIMIT 1';
    $record = db_fetch_array(db_query($sql, $params));
  }
  
  return $record;
}

/** Updateds an existing record in cache_delayed table.
 *
 * @param $qid
 *   Indentifier for the queue item to be updated. 
 * @param $cid String
 *   Cache-identifier parameter in cache_clear_all.
 * @param $table String
 *   Table parameter in cache_clear_all.
 * @param $wildcard String
 *   Wildcard parameter in cache_clear_all.
 *
 * @return
 *   Failure to write a record will return FALSE. Otherwise SAVED_UPDATED.
 */
function _cache_delayed_db_update($qid, $cid = 0, $table = '', $wildcard = '') {
  $updated_at = time();
  $res = db_query(
    "UPDATE LOW_PRIORITY {cache_delayed} SET
      cid = '%s',
      table = '%s',
      wildcard = '%s',
      updated_at = %d
    WHERE qid = %d
    ", $cid, $table, $wildcard, $updated_at, $qid);
  return ($res ? SAVED_UPDATED : FALSE); 
}

/** Removes a record from cache_delayed table.
 *
 * @param $qid
 *   Indentifier for the queue item to be updated. 
 * @param $cid String
 *   Cache-identifier parameter in cache_clear_all.
 * @param $table String
 *   Table parameter in cache_clear_all.
 * @param $wildcard String
 *   Wildcard parameter in cache_clear_all.
 *
 * @return
 *   Failure to delete a record will return FALSE. Otherwise TRUE.
 * @TODO: allow removal based on other columns in table.
 */
function _cache_delayed_db_delete($qid) {
  $res = db_query(
    "DELETE LOW_PRIORITY QUICK FROM {cache_delayed} 
    WHERE qid = %d
    ", $qid);

  return ($res ? TRUE : FALSE); 
}

