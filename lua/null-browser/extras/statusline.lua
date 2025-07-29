local html = require 'null-browser.extras.html'
local statusline = {
  config = {
    decoration = web.decorations.bottom,
    mode_labels = {
      n = 'NORMAL',
      i = 'INSERT',
      f = 'FIND',
    },
    mode_styles = {
      n = 'background-color: #007070; color: white;',
      i = 'background-color: #e06c75; color: white;',
      f = 'background-color: #51e980; color: #333;',
    },
    segments = {
      left = {
        { name = 'mode' },
        { name = 'url' },
      },
      right = {
        { name = 'views-count' },
      }
    },
  },
}

function statusline.setup(config)
  for key, value in pairs(config) do
    statusline.config[key] = value
  end
end

function statusline.init(config)
  statusline.setup(config)

  web.event.add_listener('WinCreated', {
    callback = function(event)
      local decoration = statusline.config.decoration
      decoration.enable({ win = event.win_id })
      statusline.show_status_in_window(event.win_id, decoration)
    end,
  })

  web.keymap.set('n', '<space>gg', function()
    local decoration = statusline.config.decoration
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

  web.event.add_listener({
    'ModeChanged',
    'UrlChanged',
    'ViewSelected',
    'ViewClosed',
    'ViewCreated',
  }, {
    callback = function(_) show_statusline() end,
  })
end

function statusline.current_url()
  local views = web.view.list()
  local view = views[web.view.current_index()]
  return view and view.url or ""
end

local function segment_html(segment)
  local segments_ui = {
    mode = function()
      local mode = web.keymap.get_mode()
      return html.div({ class = 'mode', style = statusline.config.mode_styles[mode] or '' }, {
        statusline.config.mode_labels[mode] or mode,
      })
    end,
    url = function()
      return html.div({ class = 'url' }, { statusline.current_url() })
    end,
    ['views-count'] = function()
      return html.div({ class = 'tab-info' }, {
        web.view.current_index() .. '/' .. #web.view.list(),
      })
    end
  }
  return segments_ui[segment.name] and segments_ui[segment.name]() or ''
end

local function map(ls, fn)
  local res = {}
  for _, x in ipairs(ls) do
    table.insert(res, fn(x))
  end
  return res
end

function statusline.html()
  return html.to_string(
    html.div({}, {
      html.style({}, { html.raw(statusline.css()) }),
      html.div({ class = 'statusline' }, {
        html.div({ class = 'statusline-segment statusline-left' },
          map(statusline.config.segments.left, segment_html)),
        html.div({ class = 'statusline-segment statusline-right' },
          map(statusline.config.segments.right, segment_html)),
      })
    })
  )
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
      overflow: hidden;
    }
    .statusline-segment {
      display: flex;
      align-items: stretch;
      max-width: 50vw;
      overflow: hidden;
      width: 100%;
    }
    .statusline-segment > * {
      display: flex;
      justify-content: flex-start;
      align-items: center;
      padding: 0 8px;
    }
    .statusline-left {
      justify-content: flex-start;
    }
    .statusline-right {
      justify-content: flex-end;
    }

    .mode {
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100%;
      font-weight: bold;
      background-color: #888;
    }
    .url {
      text-overflow: ellipsis;
      text-wrap: nowrap;
    }
    .tab-info {
      background-color: #333;
    }
  ]]
end

return statusline
