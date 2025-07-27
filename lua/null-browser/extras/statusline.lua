local html = require 'null-browser.extras.html'
local statusline = {}

local mode_labels = { n = 'NORMAL', i = 'INSERT' }

function statusline.init(opts)
  local decoration = (opts or {}).decoration or web.decorations.bottom

  web.event.add_listener('WinCreated', {
    callback = function(event)
      decoration.enable({ win = event.win_id })
      statusline.show_status_in_window(event.win_id, decoration)
    end,
  })

  web.keymap.set('n', '<space>gg', function()
    if decoration.is_enabled({ win = 0 }) then
      decoration.disable({ win = 0 })
    else
      decoration.enable({ win = 0 })
    end
  end)
end

function statusline.show_status_in_window(win_id, decoration)
  local show_statusline = function()
    local view = decoration.view({ win = win_id })
    if view == nil then return end
    web.view.set_html(statusline.html(), { view = view })
  end

  web.schedule(function()
    local view = decoration.view({ win = win_id })
    -- FIXME: web.schedule doesn't guarantee that view will be available
    if not view then
      web.schedule(function() show_statusline() end)
    else
      show_statusline()
    end
  end)
  web.event.add_listener({ 'ModeChanged', 'UrlChanged', 'ViewSelected', 'ViewClosed', 'ViewCreated' }, {
    callback = function(_) show_statusline() end,
  })
end

local mode_styles = {
  n = 'background-color: #007070; color: white;',
  i = 'background-color: #e06c75; color: white;',
}

function statusline.current_url()
  local current_view = web.view.current();
  for _, view in ipairs(web.view.list()) do
    if view.id == current_view then
      return view.url
    end
  end
  return ""
end

function statusline.html()
  local mode = web.keymap.get_mode()
  local url = statusline.current_url()
  local html_elem = html.div({}, {
    html.style({}, { html.raw(statusline.css()) }),
    html.div({ class = 'statusline' }, {
      html.div({ class = 'mode', style = mode_styles[mode] or '' }, {
        mode_labels[mode] or mode,
      }),
      html.div({ class = 'url' }, { url }),
    })
  })
  return html.to_string(html_elem)
end

function statusline.css()
  return [[
    html, body {
      background: #000;
    }
    .statusline {
      width: 100%;
      height: 100vh;
      display: flex;
      justify-content: space-between;
      align-items: stretch;
    }
    .mode {
      display: flex;
      justify-content: center;
      align-items: center;
      padding: 0 8px;
      font-weight: bold;
      background-color: #888;
    }
    .url {
      display: flex;
      justify-content: flex-start;
      align-items: center;
      padding: 0 8px;
      max-width: 50vw;
      overflow: hidden;
      text-overflow: ellipsis;
      text-wrap: nowrap;
    }
  ]]
end

return statusline
