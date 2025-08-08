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
search_engines.urls['r'] = 'https://reddit.com/r/{}'

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

-- Tabline
require 'null-browser.extras.tabline'.init {
  decoration = web.decorations.top,
}

-- Statusline
require 'null-browser.extras.statusline'.init {
  decoration = web.decorations.bottom,
}


web.keymap.set('n', '<c-h>', function()
  local symbols = web.help.get_items()
  menu:select(symbols, { select_last_line = true }, function(err, selection)
    if err then return end
    web.help.show(web.utils.string_trim(selection))
  end)
end)

local hints = require 'null-browser.extras.hints'
hints.init()
web.keymap.set('n', 'f', function() hints.start('a[href], button', hints.action.open_in_view) end)
web.keymap.set('n', '<s-f>', function() hints.start('a[href], button', hints.action.open_in_new_view) end)
web.keymap.set('n', 'yl', function() hints.start('a[href]', hints.action.copy_link) end)

print('ending...')
