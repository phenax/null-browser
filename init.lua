print('hello starting up...')

web.opts.downloads_dir = os.getenv('HOME') .. '/Downloads/firefox'
web.opts.permissions_persistance = 'never'
web.opts.user_agent =
'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36'
web.opts.close_window_when_no_views = true
web.opts.new_view_url = 'https://lite.duckduckgo.com'

local history = require 'null-browser.extras.history'
history.attach_hooks()

local search_engines = require 'null-browser.extras.search-engines'
search_engines.urls['ld'] = 'https://lite.duckduckgo.com/?q={}'
search_engines.urls['g'] = 'https://github.com/{}'
search_engines.urls['y'] = 'https://youtube.com/results?search_query={}'
search_engines.urls['r'] = 'https://reddit.com/{}'

local Dmenu = require 'null-browser.extras.dmenu'
local menu = Dmenu:new {
  select_last_line = false,
  args = { '-r' },
  transform_output = function(selections)
    if type(selections) ~= "table" then return selections end
    if #selections <= 1 then return selections[1] end
    return selections[#selections - 1]
  end,
  -- command = os.getenv('HOME') .. '/scripts/fzfmenu.sh',
  -- prompt_arg = '--prompt',
  -- query_arg = '-q',
  -- args = { 'input', '--layout=default', '--print-query' },
}

require 'null-browser.defaults.vi'.setup {
  menu = menu,
  history = history,
  transform_url_input = search_engines.transform_url_input,
}

-- web.keymap.set('zoom', 'j', function() web.view.zoom_out() end)
-- web.keymap.set('zoom', 'k', function() web.view.zoom_in() end)
-- web.keymap.set('zoom', '<esc>', function() web.keymap.set_mode('n') end)
-- web.keymap.set('n', 'z', function() web.keymap.set_mode('zoom') end)

web.event.add_listener('NotificationReceived', {
  callback = function(event)
    -- TODO: Add action/click (-A)
    local args = { '-a', 'null-browser', '-r', event.tag, event.title, event.message }
    web.uv.spawn('notify-send', { args = args }, function(code)
      if code ~= 0 then
        print('[notify-send] Exit with status code: ' .. code)
      end
    end)
  end,
})

local start_clock = function(win_id)
  local timer = web.uv.new_timer()
  timer:start(500, 1000, function()
    local view = web.decorations.bottom.view({ win = win_id })
    if view ~= nil then
      local time = os.date("%X", os.time())
      web.view.set_html('Time: ' .. time, { view = view })
    end
  end)
end

require 'null-browser.extras.tabline'.init()

-- Decorations config
-- web.event.add_listener('WinCreated', {
--   callback = function(event)
--     web.decorations.bottom.enable({ win = event.win_id })
--     start_clock(event.win_id)
--   end,
-- })

web.keymap.set('n', '<space>gg', function()
  if web.decorations.bottom.is_enabled({ win = 0 }) then
    web.decorations.bottom.disable({ win = 0 })
  else
    web.decorations.bottom.enable({ win = 0 })
  end
end)

print('ending...')
