print('hello starting up...')

local dmenu = require 'null-browser.extras.dmenu'
local history = require 'null-browser.extras.history'

--- @type table
web = web
--- @type table
uv = uv

local function get_current_view_index()
  local currentView = web.view.current();
  for index, view in ipairs(web.view.list()) do
    if view.id == currentView then
      return index
    end
  end
end

web.set('new_view_url', 'https://lite.duckduckgo.com')
web.set('close_window_when_no_views', true)
web.set('user_agent', 'MacOS | Safari - $500 edition')
web.set('downloads_dir', os.getenv('HOME') .. '/Downloads/firefox')

web.event.add_listener('UrlChanged', {
  callback = function(opts)
    print('url change', web.inspect(opts));
    history.add(opts.url)
  end
})

local function trim(s)
  local res, _ = string.gsub(s, '^%s*(.-)%s*$', '%1')
  return res
end

local function to_url(url)
  if string.match(url, "^https?://") then
    return trim(url)
  end
  return "https://" .. trim(url)
end

-- Search
web.keymap.set('n', '<esc>', function() web.search.reset() end)
web.keymap.set('n', 'n', function() web.search.next() end)
web.keymap.set('n', '<s-n>', function() web.search.previous() end)
web.keymap.set('n', '<c-f>', function()
  dmenu.input({ prompt = 'Search:', input = web.search.get_text() }, function(err, input)
    if err then return end
    web.search.set_text(input)
  end)
end)

-- Open in new view
web.keymap.set('n', 'o', function()
  print(web.get('new_view_url'), web.get('user_agent'))
  dmenu.select(history.list(), { prompt = 'Open view:' }, function(err, result)
    if err or not result then return end
    web.view.create(to_url(result))
  end)
end)
-- Open in current view
web.keymap.set('n', '<s-o>', function()
  dmenu.select(history.list(), { prompt = 'Open:' }, function(err, result)
    if err or not result then return end
    web.view.set_url(to_url(result))
  end)
end)
-- Delete from history
web.keymap.set('n', '<c-h>d', function()
  dmenu.select(history.list(), { prompt = 'Delete history:' }, function(err, result)
    if err or not result then return end
    history.delete(trim(result))
  end)
end)
-- Update current url
web.keymap.set('n', '<c-l>', function()
  local views = web.view.list()
  local view = views[get_current_view_index()];
  if view == nil then return end
  dmenu.select(history.list(), { prompt = 'Set url:', input = view.url }, function(err, result)
    if err or not result then return end
    web.view.set_url(trim(result))
  end)
end)

-- Run lua code
web.keymap.set('n', 'q', function()
  dmenu.input({ prompt = 'Lua:' }, function(err, result)
    if err or not result then return end
    local run, run_err = load(result)
    if run_err then print(run_err) end
    if run then run() end
  end)
end)

-- History back/forward
web.keymap.set('n', '<s-h>', function() web.history.back(); end)
web.keymap.set('n', '<s-l>', function() web.history.forward(); end)

-- Close view
web.keymap.set('n', '<c-w>', function() web.view.close(); end)

-- view select
web.keymap.set('n', 'b', function()
  local views_list = {}
  local views = web.view.list()
  for index, view in ipairs(web.view.list()) do
    table.insert(views_list, index .. ': ' .. view.title .. ' (' .. view.url .. ')')
  end
  dmenu.select(views_list, { prompt = 'Views:' }, function(err, result)
    if err or not result then return end
    local index_str, _ = trim(result):gsub('%s*:.*$', '')
    local index = tonumber(index_str)
    if views[index] then
      web.view.select(views[index].id)
    end
  end)
end)

-- 1-0 for tab indexes 1-10
for index = 1, 10 do
  local key = index
  if index >= 10 then key = 0 end
  web.keymap.set('n', '<space>' .. key, function()
    local views = web.view.list()
    if index > #views then return end
    web.view.select(views[index].id)
  end)
end

-- Next view
web.keymap.set('n', 'tn', function()
  local views = web.view.list()
  if #views <= 1 then return end
  local index = get_current_view_index() + 1;
  if index > #views then index = 1 end
  web.view.select(views[index].id)
end)

-- Prev view
web.keymap.set('n', 'tp', function()
  local views = web.views.list()
  if #views <= 1 then return end
  local index = get_current_view_index() - 1;
  if index <= 0 then index = #views end
  web.views.select(views[index].id)
end)

web.keymap.set('n', '<c-i>', function() web.view.open_devtools() end)

print('ending...')
