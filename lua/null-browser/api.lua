--- Internal api (unstable)
--- @type table
__internals = __internals

--- @type table
_G.web = _G.web or {}
web.search = web.search or {}
web.keymap = web.keymap or {}
web.view = web.view or {}
web.history = web.history or {}
web.event = web.event or {}

require 'null-browser.utils'

--- Add a keymap
---
--- @param mode   string           Keymap mode ("i", "n", ...)
--- @param key    string           Key sequence (Eg: `<c-t>j`)
--- @param action function         Function to run when keymap is triggered
---
--- @example
--- ```lua
--- web.keymap.set('n', 'o', function()
---   web.view.create('https://google.com')
--- end)
--- ```
function web.keymap.set(mode, key, action) return __internals.keymap_set(mode, key, action) end

--- Set the current keymap mode
---
--- @param mode string    The keymap mode to set
---
--- @example
--- ```lua
--- web.keymap.set_mode('n') -- Set to normal mode
--- web.keymap.set_mode('i') -- Set to insert mode
--- ```
function web.keymap.set_mode(mode) return __internals.keymap_set_mode(mode) end

--- Get the current keymap mode
---
--- @example
--- ```lua
--- local current_mode = web.keymap.get_mode()
--- ```
function web.keymap.get_mode() return __internals.keymap_get_mode() end

--- Close view
---
--- @param view_id?  number    View id to close
---
--- @example
--- ```lua
--- web.view.close()  -- Close current view in active window
--- web.view.close(3) -- Close view with id 3
--- ```
function web.view.close(view_id) return __internals.view_close(view_id) end

--- Create a new view with a url
---
--- @param url      string    Url to open in the new view
---
--- @example
--- ```lua
--- web.view.create('https://duckduckgo.com') -- Opens url in new view
--- web.view.create() -- Opens new_view_url by default
--- ```
function web.view.create(url) return __internals.view_create(url) end

--- Get current view id
---
--- @return number view_id Current view id
---
--- @example
--- ```lua
--- local view_id = web.view.current()
--- ```
function web.view.current() return __internals.view_current() end

--- Get a list of views in the current window
---
--- @return table views  List of views
---
--- @example
--- ```lua
--- local views = web.view.list()
--- print(views[1].url, views[1].title, views[1].id)
--- ```
function web.view.list() return __internals.view_list() end

--- Select a view in current window
---
--- @param view_id number View id to select
---
--- @example
--- ```lua
--- web.view.select(3) -- Select view with id 3
--- ```
function web.view.select(view_id) return __internals.view_select(view_id) end

--- Set url of a given view
---
--- @param url string      Url to open
--- @param view_id? number  View id to select
---
--- @example
--- ```lua
--- web.view.set_url('https://foobar.com')
--- web.view.set_url('https://foobar.com', 3) -- Set url for view with id 3
--- ```
function web.view.set_url(url, view_id) return __internals.view_set_url(url, view_id) end

--- Open devtools window for the view
---
--- @param view_id? number    Id of the view
---
--- @example
--- ```lua
--- web.view.open_devtools() -- Open devtools window for current view
--- web.view.open_devtools(5) -- Open devtools window for view id 5
--- ```
function web.view.open_devtools(view_id) return __internals.view_open_devtools(view_id) end

--- Get current view index
---
--- @return number|nil index Current view index
---
--- @example
--- ```lua
--- local index = web.view.current_index()
--- ```
function web.view.current_index()
  local current_view = web.view.current();
  for index, view in ipairs(web.view.list()) do
    if view.id == current_view then
      return index
    end
  end
  return nil
end

--- Listen to events from the browser
---
--- @class nullb.EventListenerOptions
--- @field callback any  Callback called when that event occurs
---
--- @param events string|table             Event or events to listen to
--- @param opts nullb.EventListenerOptions  Options
---
--- @example
--- ```lua
--- web.event.add_listener('UrlChanged', {
---   callback = function(opts)
---     print(opts.url, opts.tab, opts.win)
---   end,
--- })
--- ```
function web.event.add_listener(events, opts)
  opts = web.utils.table_merge({}, opts or {})
  if type(events) == "string" then events = { events } end
  opts.events = events or {}

  return __internals.event_add_listener(opts)
end

--- Set configuration options
---
--- @param key string                        The name of the configuration
--- @param value nil|string|boolean|number   Configuration value
---
--- @example
--- ```lua
--- web.set('new_view_url', 'https://duckduckgo.com')
--- ```
function web.set(key, value) __internals.config_set(key, value) end

--- Web options. Calls web.set/web.get for options
web.opts = setmetatable({}, {
  __index = function(_, key)
    return web.get(key)
  end,
  __newindex = function(_, key, value)
    web.set(key, value)
  end,
})

--- Get configuration value
---
--- @param key string The name of the configuration
---
--- @example
--- ```lua
--- local url = web.get('new_view_url')
--- ```
function web.get(key) return __internals.config_get(key) end

--- Go back in history for given view
---
--- @param count?   number     Number of history items to go back in (default = 1)
--- @param view_id? number   Id of the view
---
--- @example
--- ```lua
--- web.history.back(2, 3) -- Go back 2 history items for view id 3
--- ```
function web.history.back(count, view_id) return __internals.history_back(count, view_id) end

--- Go forward in history for given view
---
--- @param count?   number     Number of history items to go forward in (default = 1)
--- @param view_id? number   Id of the view
---
--- @example
--- ```lua
--- web.history.forward(2, 3) -- Go forward 2 history items for view id 3
--- ```
function web.history.forward(count, view_id) return __internals.history_forward(count, view_id) end

--- Search text inside a view
---
--- @param text    string     Text to search
--- @param view_id? number    Id of the view
---
--- @example
--- ```lua
--- web.search.set_text('whatever') -- Search in current view
--- web.search.set_text('whatever', 5) -- Search in view id 5
--- ```
function web.search.set_text(text, view_id) return __internals.search_set_text(text, view_id) end

--- Reset searched text in a view (Same as web.search.set_text(''))
---
--- @param view_id? number    Id of the view
---
--- @example
--- ```lua
--- web.search.reset() -- Reset search state for current view
--- web.search.reset(5) -- Reset search state for view id 5
--- ```
function web.search.reset(view_id) return __internals.search_set_text('', view_id) end

--- Get the last searched text
---
--- @example
--- ```lua
--- local text = web.search.get_text()
--- ```
function web.search.get_text() return __internals.search_get_text() end

--- Highlight next search term for the last searched text
---
--- @param view_id? number    Id of the view
---
--- @example
--- ```lua
--- web.search.next() -- Next search result in current view
--- web.search.next(5) -- Next search result in view id 5
--- ```
function web.search.next(view_id) return __internals.search_next(view_id) end

--- Highlight previous search term for the last searched text
---
--- @param view_id? number    Id of the view
---
--- @example
--- ```lua
--- web.search.previous() -- Previous search result in current view
--- web.search.previous(5) -- Previous search result in view id 5
--- ```
function web.search.previous(view_id) return __internals.search_previous(view_id) end

--- TODO: Document
function web.view.scroll(deltax, deltay, view_id) return __internals.view_scroll(deltax, deltay, view_id) end

--- TODO: Document
function web.view.scroll_to_top(view_id) return __internals.view_scroll_to_top(view_id) end

--- TODO: Document
function web.view.scroll_to_bottom(view_id) return __internals.view_scroll_to_bottom(view_id) end

print("api loaded")
