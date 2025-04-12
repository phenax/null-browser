print('hello starting up...')

--- @type table
web = web
--- @type table
uv = uv

local function get_current_tab_index()
  local currentTab = web.tabs.current();
  for index, tab in ipairs(web.tabs.list()) do
    if tab.id == currentTab then
      return index
    end
  end
end

local dmenu = require 'null-browser.extras.dmenu'
local history = require 'null-browser.extras.history'

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

-- Open in new tab
web.keymap.set('n', 'o', function()
  dmenu.select(history.list(), { prompt = 'Open tab:' }, function(err, result)
    if err or not result then return end
    web.tabs.new(to_url(result))
  end)
end)
-- Open in current tab
web.keymap.set('n', '<s-o>', function()
  dmenu.select(history.list(), { prompt = 'Open:' }, function(err, result)
    if err or not result then return end
    web.tabs.set_url(to_url(result))
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
  local tabs = web.tabs.list()
  local tab = tabs[get_current_tab_index()];
  if tab == nil then return end
  dmenu.select(history.list(), { prompt = 'Set url:', input = tab.url }, function(err, result)
    if err or not result then return end
    web.tabs.set_url(trim(result))
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

-- Close tab
web.keymap.set('n', '<c-w>', function() web.tabs.close(); end)

-- Tab select
web.keymap.set('n', 'b', function()
  local tabs_list = {}
  local tabs = web.tabs.list()
  for index, tab in ipairs(web.tabs.list()) do
    table.insert(tabs_list, index .. ': ' .. tab.title .. ' (' .. tab.url .. ')')
  end
  dmenu.select(tabs_list, { prompt = 'Tab:' }, function(err, result)
    if err or not result then return end
    local index_str, _ = trim(result):gsub('%s*:.*$', '')
    local index = tonumber(index_str)
    if tabs[index] then
      web.tabs.select(tabs[index].id)
    end
  end)
end)

-- Next tab
web.keymap.set('n', 'tn', function()
  local tabs = web.tabs.list()
  if #tabs <= 1 then return end
  local index = get_current_tab_index() + 1;
  if index > #tabs then index = 1 end
  web.tabs.select(tabs[index].id)
end)

-- Prev tab
web.keymap.set('n', 'tp', function()
  local tabs = web.tabs.list()
  if #tabs <= 1 then return end
  local index = get_current_tab_index() - 1;
  if index <= 0 then index = #tabs end
  web.tabs.select(tabs[index].id)
end)

print('ending...')
