---@diagnostic disable: doc-field-no-class, undefined-doc-param
--- Lua api for configuring null browser.
--- @module web


--- Internal api (unstable)
_G.__internals = _G.__internals
_G.web = _G.web or {}
web.search = web.search or {}
web.keymap = web.keymap or {}
web.view = web.view or {}
web.history = web.history or {}
web.event = web.event or {}
web.decorations = web.decorations or {}
web.help = web.help or {}

require 'null-browser.utils'

--- Schedule a function to be called on next tick (qt+libuv event loop)
---
--- @param fn fun():nil   Function to call
function web.schedule(fn) __internals.schedule(fn) end

--- Add a keymap
---
--- @param mode   string           Keymap mode ("i", "n", ...)
--- @param key    string           Key sequence (Eg: `<c-t>j`)
--- @param action function         Function to run when keymap is triggered
---
--- @usage
--- web.keymap.set('n', 'o', function()
---   web.view.create('https://google.com')
--- end)
function web.keymap.set(mode, key, action) return __internals.keymap_set(mode, key, action) end

--- Set the current keymap mode
---
--- @param mode string    The keymap mode to set
---
--- @usage
--- web.keymap.set_mode('n') -- Set to normal mode
--- web.keymap.set_mode('i') -- Set to insert mode
function web.keymap.set_mode(mode) return __internals.keymap_set_mode(mode) end

--- Get the current keymap mode
---
--- @usage
--- local current_mode = web.keymap.get_mode()
function web.keymap.get_mode() return __internals.keymap_get_mode() end

--- Close view
---
--- @param opts table?        Options
--- @param opts.view number?  View id (default = 0)
---
--- @usage
--- web.view.close()  -- Close current view in active window
--- web.view.close({ view = 3 }) -- Close view with id 3
function web.view.close(opts) return __internals.view_close((opts or {}).view) end

--- Create a new view with a url
---
--- @param url      string    Url to open in the new view
---
--- @usage
--- web.view.create('https://duckduckgo.com') -- Opens url in new view
--- web.view.create() -- Opens new_view_url by default
function web.view.create(url) return __internals.view_create(url) end

--- Get current view id
---
--- @return number view_id Current view id
---
--- @usage
--- local view_id = web.view.current()
function web.view.current() return __internals.view_current() end

--- Get current url
---
--- @return string url Current view url
---
--- @usage
--- local url = web.view.current_url()
function web.view.current_url()
  local views = web.view.list()
  local view = views[web.view.current_index()];
  return view and view.url
end

--- Get a list of views in the current window
---
--- @return table views  List of views
---
--- @usage
--- local views = web.view.list()
--- print(views[1].url, views[1].title, views[1].id)
function web.view.list() return __internals.view_list() end

--- Select a view in current window
---
--- @param view_id number View id to select
---
--- @usage
--- web.view.select(3) -- Select view with id 3
function web.view.select(view_id) return __internals.view_select(view_id) end

--- Set url of a given view
---
--- @param url string         Url to open
--- @param opts table?        Options
--- @param opts.view number?  View id (default = 0)
---
--- @usage
--- web.view.set_url('https://foobar.com')
--- web.view.set_url('https://foobar.com', { view = 3 }) -- Set url for view with id 3
function web.view.set_url(url, opts) return __internals.view_set_url(url, (opts or {}).view) end

--- Set html inside the page for a given view
---
--- @param html       string   HTML string
--- @param opts?      table    Options
--- @param opts.view number?   View id (default = 0)
---
--- @usage
--- web.view.set_html('<h2>HJello</h2>')
--- web.view.set_html('<h2>HJello</h2>', { view = 3 }) -- Set html for view with id 3
function web.view.set_html(html, opts) return __internals.view_set_html(html, (opts or {}).view) end

--- Run js in a view
---
--- @param js string                         JS string
--- @param opts table?                       Options
--- @param opts.view number?                 View id (default = 0)
--- @param opts.on_result? fun(v: any): nil  Callback with serialized result of evaluation
---
--- @usage
--- web.view.run_js('console.log(42)')
--- web.view.run_js('console.log(42)', { view = 3 }) -- Set html for view with id 3
--- web.view.run_js('2 * 3', {
---   on_result = function(result)
---     print(result * 7)
---   end,
--- })
function web.view.run_js(js, opts) return __internals.view_run_js(js, opts or {}) end

--- @class ReloadOpts
--- @field view? number View id

--- Reload a given view
---
--- @param opts table?           Options
--- @param opts.view number?     View id (default = 0)
---
--- @usage
--- web.view.reload()
--- web.view.reload({ view = 3 }) -- Reload view with id 3
function web.view.reload(opts) return __internals.view_reload((opts or {}).view) end

--- @class ExposeOpts
--- @field view? number View id (default = 0)

--- Expose a lua function inside a view (Only works with decorations)
---
--- @param name    string           Func name
--- @param action  fun(table):nil   Action to call when function is invoked in view
--- @param opts table?              Options
--- @param opts.view number?        View id (default = 0)
---
--- @usage
--- web.view.expose('tabSelect', function(args)
---   web.view.select(tonumber(args.view))
--- end)
---
--- @usage
--- -- In javascript
--- _nullbrowser.rpc.tabSelect({ view: 5 })
function web.view.expose(name, action, opts) return __internals.view_expose(name, action, (opts or {}).view) end

--- Open devtools window for the view
---
--- @param opts table?              Options
--- @param opts.view number?        View id (default = 0)
---
--- @usage
--- web.view.open_devtools() -- Open devtools window for current view
--- web.view.open_devtools(5) -- Open devtools window for view id 5
function web.view.open_devtools(opts) return __internals.view_open_devtools((opts or {}).view) end

--- Get current view index
---
--- @return number|nil index Current view index
---
--- @usage
--- local index = web.view.current_index()
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
--- @param events string|table              Event or events to listen to
--- @param opts table                       Options
--- @param opts.callback fun(opts:any):nil  Callback called when that event occurs
---
--- @see WebEvents
---
--- @usage
--- web.event.add_listener('UrlChanged', {
---   callback = function(opts)
---     print(opts.url, opts.view, opts.win)
---   end,
--- })
function web.event.add_listener(events, opts)
  opts = web.utils.table_merge({}, opts or {})
  if type(events) == "string" then events = { events } end
  opts.events = events or {}

  return __internals.event_add_listener(opts)
end

--- Set configuration options
--- @see WebOpts
---
--- @param key string                        The name of the configuration
--- @param value nil|string|boolean|number   Configuration value
---
--- @usage
--- web.set('new_view_url', 'https://duckduckgo.com')
--- -- Can also be written as
--- web.opts.new_view_url = 'https://duckduckgo.com'
function web.set(key, value) __internals.config_set(key, value) end

--- Get configuration value
--- @see WebOpts
---
--- @param key string The name of the configuration
---
--- @usage
--- local url = web.get('new_view_url')
function web.get(key) return __internals.config_get(key) end

--- Maps to values defined in ./src/Configuration.hpp
--- @table WebOpts
--- @field app_data_dir string                 (readonly) Directory where app data is stored
--- @field close_window_when_no_views boolean  If true, when the last tab in a window is closed, the window closes (default: true)
--- @field config_dir string                   (readonly) Directory where app config is stored
--- @field downloads_dir string                Directory where downloaded files go
--- @field new_view_url string                 URL used when view url is not specified (default: 'https://duckduckgo.com')
--- @field null_assets_dir string              (readonly) Directory where null assets are installed
--- @field null_docs_dir string                (readonly) Directory where null docs are installed
--- @field permissions_persistance 'always'|'session'|'never'  How to persist choices for permissions (default: 'always')
--- @field user_agent string                   User agent sent by the browser

--- API for configuring browser options
--- @see WebOpts
---
--- @usage
--- web.opts.new_view_url = 'https://duckduckgo.com'
web.opts = setmetatable({}, {
  __index = function(_, key)
    return web.get(key)
  end,
  __newindex = function(_, key, value)
    web.set(key, value)
  end,
})

--- Go back in history for given view
---
--- @param count   number?     Number of history items to go back in (default = 1)
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
---
--- @usage
--- web.history.back(2, { view = 3 }) -- Go back 2 history items for view id 3
function web.history.back(count, opts) return __internals.history_back(count, (opts or {}).view) end

--- Go forward in history for given view
---
--- @param count   number?     Number of history items to go forward in (default = 1)
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
---
--- @usage
--- web.history.forward(2, { view = 3 }) -- Go forward 2 history items for view id 3
function web.history.forward(count, opts) return __internals.history_forward(count, (opts or {}).view) end

--- Search text inside a view
---
--- @param text    string?     Text to search
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
---
--- @usage
--- web.search.set_text('whatever') -- Search in current view
--- web.search.set_text('whatever', { view = 5 }) -- Search in view id 5
function web.search.set_text(text, opts) return __internals.search_set_text(text, (opts or {}).view) end

--- Reset searched text in a view (Same as web.search.set_text(''))
---
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
---
--- @usage
--- web.search.reset() -- Reset search state for current view
--- web.search.reset({ view = 5 }) -- Reset search state for view id 5
function web.search.reset(opts) return __internals.search_set_text('', (opts or {}).view) end

--- Get the last searched text
---
--- @usage
--- local text = web.search.get_text()
function web.search.get_text() return __internals.search_get_text() end

--- Highlight next search term for the last searched text
---
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
---
--- @usage
--- web.search.next() -- Next search result in current view
--- web.search.next({ view = 5 }) -- Next search result in view id 5
function web.search.next(opts) return __internals.search_next((opts or {}).view) end

--- Highlight previous search term for the last searched text
---
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
---
--- @usage
--- web.search.previous() -- Previous search result in current view
--- web.search.previous(5) -- Previous search result in view id 5
function web.search.previous(opts) return __internals.search_previous((opts or {}).view) end

--- TODO: Document
--- @param deltax number
--- @param deltay number
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
function web.view.scroll(deltax, deltay, opts) return __internals.view_scroll(deltax, deltay, (opts or {}).view) end

--- TODO: Document
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
function web.view.scroll_to_top(opts) return __internals.view_scroll_to_top((opts or {}).view) end

--- TODO: Document
--- @param opts table?         Options
--- @param opts.view number?   View id (default = 0)
function web.view.scroll_to_bottom(opts) return __internals.view_scroll_to_bottom((opts or {}).view) end

--- Maps to values defined in ./src/widgets/Decorations.hpp
local DecorationType = { top = 1, bottom = 2, left = 3, right = 4 }

--- @table DecorationOpts
--- @field win? number

--- @table Decoration
--- @field type fun(): 'left'|'right'|'top'|'bottom'
--- @field enable fun(opts?: DecorationOpts): nil
--- @field disable fun(opts?: DecorationOpts): nil
--- @field set_enabled fun(enabled: boolean, opts?: DecorationOpts): nil
--- @field is_enabled fun(opts?: DecorationOpts): boolean
--- @field view fun(opts?: DecorationOpts): number|nil
--- @field set_size fun(size: number, opts?: DecorationOpts): nil

--- Decoration api
---
--- @param type 'left'|'right'|'top'|'bottom'
--- @return Decoration
local function create_decoration_api(type)
  local type_id = DecorationType[type]
  local set_enabled = function(enabled, opts)
    __internals.decorations_set_enabled(type_id, enabled, (opts or {}).win)
  end;
  return {
    type = function() return type end,
    enable = function(opts) set_enabled(true, opts) end,
    disable = function(opts) set_enabled(false, opts) end,
    is_enabled = function(opts)
      return __internals.decorations_get_enabled(type_id, (opts or {}).win)
    end,
    view = function(opts)
      return __internals.decorations_get_view(type_id, (opts or {}).win)
    end,
    set_size = function(size, opts) __internals.decorations_set_size(type_id, size, (opts or {}).win) end,
  }
end

web.decorations.top = create_decoration_api('top')
web.decorations.bottom = create_decoration_api('bottom')
web.decorations.left = create_decoration_api('left')
web.decorations.right = create_decoration_api('right')

--- Get a list of items for help
--- @return table<string> help_items List of help item names
function web.help.get_items()
  local symbols_json = web.opts.null_docs_dir .. '/api/symbols.json'
  local file, _ = io.open(symbols_json, 'r')
  if file then
    local contents, _ = file:read('a')
    return web.json.decode(contents)
  end
  return {}
end

--- Open help docs for a given item
---
--- @param item string? Help item name
--- @param opts table? Options
--- @param opts.view number?  View id (default = 0)
function web.help.show(item, opts)
  local url = 'null://docs/api/index.html#' .. (item or '')
  if opts and opts.view ~= nil then
    web.view.set_url(url, opts)
  else
    web.view.create(url)
  end
end
