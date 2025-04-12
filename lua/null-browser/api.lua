--- @type table
__internals = __internals

--- @type table
_G.web = _G.web or {
  keymap = {},
  view = {},
  history = {},
  event = {},
}

local function shallow_copy(t)
  local t2 = {}
  for k, v in pairs(t) do t2[k] = v end
  return t2
end

-- inspect lua tables
local inspector_loaded, inspector = pcall(require, 'inspect')
if inspector_loaded then
  web.inspect = inspector.inspect
else
  web.inspect = function(val)
    print('[warn] "inspect" module not loaded'); return val
  end
end

--- Add a keymap
---
--- @param mode   string|table     Mode or a list of modes ("i", "n")
--- @param key    string           Key sequence (Eg: `<c-t>j`)
--- @param action function         Function to run when keymap is triggered
---
--- @example
--- ```lua
--- web.keymap.set('n', 'o', function()
---   web.view.create('https://google.com') -- Open google in new view
--- end)
--- ```
function web.keymap.set(mode, key, action) return __internals.keymap_set(mode, key, action) end

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
  opts = shallow_copy(opts or {})
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

print("api loaded")
