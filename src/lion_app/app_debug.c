#include <lion/lion.h>
#include <lion_utils/vendor/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG

// Since this is the debug information we can't track allocations or an infinite
// loop is formed

_idebug_heap_info_t *heapinfo_new(lion_app_t *app) {
  logi_trace("Creating new node with heap info");
  _idebug_heap_info_t *node = malloc(sizeof(_idebug_heap_info_t));
  if (node == NULL) {
    logi_error("Could not allocate memory for node");
    return NULL;
  }
  node->addr  = NULL;
  node->size  = 0;
  *node->file = 0;
  node->line  = -1;
  node->next  = NULL;
  return node;
}

void heapinfo_free_node(_idebug_heap_info_t *node) { free(node); }

void heapinfo_clean(lion_app_t *app) {
  logi_trace("Removing heap info");
  _idebug_heap_info_t *node = app->_idebug_heap_head;
  _idebug_heap_info_t *next;
  while (node != NULL) {
    next = node->next;
    heapinfo_free_node(node);
    node = next;
  }
}

void heapinfo_push(lion_app_t *app, void *addr, size_t size, const char *file, int line) {
  logi_trace("Pushing %#p @ %s:%d", addr, file, line);
  _idebug_heap_info_t *head  = app->_idebug_heap_head;
  size_t               count = 0;
  if (head->addr == NULL) {
    logi_trace("Pushing to start of list");
    head->addr = addr;
    head->size = size;
    strncpy(head->file, file, FILENAME_MAX);
    head->line = line;
    logi_trace("Count after push is %d", count + 1);
    return;
  }
  count++;

  while (head->next != NULL) {
    count++;
    head = head->next;
  }
  _idebug_heap_info_t *node = heapinfo_new(app);
  if (node == NULL) {
    logi_error("Could not push element");
    return;
  }

  node->addr = addr;
  node->size = size;
  strncpy(node->file, file, FILENAME_MAX);
  node->line = line;
  head->next = node;
  logi_trace("Count after push is %d", count + 1);
}

size_t heapinfo_popaddr(lion_app_t *app, void *addr) {
  logi_trace("Searching element with address %#p", addr);
  size_t               count = 0;
  _idebug_heap_info_t *curr  = app->_idebug_heap_head;
  if (curr->addr == addr) {
    // First element is the one to pop (border case)
    logi_trace("HEAP INFO (%d) %#p", count, curr->addr);
    logi_trace("Popping %#p @ %s:%d", curr->addr, curr->file, curr->line);
    app->_idebug_heap_head = curr->next;
    size_t size            = curr->size;
    heapinfo_free_node(curr);
    return size;
  }

  while (curr->next != NULL) {
    logi_trace("HEAP INFO (%d) %#p", count, curr->addr);
    if (curr->next->addr == addr) {
      _idebug_heap_info_t *node = curr->next;
      logi_trace("HEAP INFO (%d) %#p", count + 1, node->addr);
      logi_trace("Popping %#p @ %s:%d", node->addr, node->file, node->line);
      curr->next  = node->next;
      size_t size = node->size;
      heapinfo_free_node(node);
      return size;
    }
    curr = curr->next;
    count++;
  }

  logi_error("Could not find element %#p (%#p)", addr, curr->addr);
  return 0;
}

size_t heapinfo_count(lion_app_t *app) {
  _idebug_heap_info_t *head  = app->_idebug_heap_head;
  size_t               count = 0;
  while (head != NULL) {
    count++;
    head = head->next;
  }
  return count;
}

#endif
