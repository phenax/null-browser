local hints = {
  config = {
    mode = 'f',
  },
  action = {},
}

local state = { action = nil }

local js_setup_code = ''

function hints.init(on_ready)
  web.keymap.set(hints.config.mode, '<Esc>', function()
    hints.stop()
  end)

  hints._load_hints_js(function()
    web.event.add_listener('KeyPressed', {
      callback = function(event)
        if web.keymap.get_mode() == hints.config.mode then
          hints._filter_key(event.key)
        end
      end,
    })

    if type(on_ready) == 'function' then on_ready() end
  end)
end

function hints.start(selector, action)
  state.action = action or hints.action.open_in_view
  web.view.run_js(
    js_setup_code ..
    ";_nullbrowser.hints.start('" .. selector .. "')"
  )
  web.schedule(function()
    -- Trigger f mode after tick to avoid the trigger key (f) to get captured in event
    web.keymap.set_mode(hints.config.mode)
  end)
end

function hints._filter_key(key)
  web.view.run_js("_nullbrowser.hints.filterOutByKey('" .. key .. "')", {
    on_result = function(find_status)
      if find_status ~= nil then hints.stop() end
      if find_status and state.action then
        state.action()
      end
    end,
  })
end

function hints.stop()
  web.keymap.set_mode('n')
  web.view.run_js('_nullbrowser.hints.stop()')
end

function hints._load_hints_js(on_ready)
  local hints_js = web.opts.null_assets_dir .. '/javascript/hints.js'
  web.uv.fs_open(hints_js, 'r', 438, function(err, file)
    if err then return end
    if not file then return {} end
    local stat = assert(web.uv.fs_fstat(file))
    local js = assert(web.uv.fs_read(file, stat.size))
    assert(web.uv.fs_close(file))
    js_setup_code = js

    on_ready()
  end)
end

function hints.action.open_in_new_view()
  web.view.run_js [[
    const match = _nullbrowser.hints.currentMatch;
    if (match.elem?.href) {
      window.open(match.elem.href)
    } else {
      match.elem?.click()
    }
  ]]
end

function hints.action.open_in_view()
  web.view.run_js [[
    const match = _nullbrowser.hints.currentMatch;
    if (match.elem?.href) {
      location.href = match.elem.href
    } else {
      match.elem?.click()
    }
  ]]
end

function hints.action.focus()
  web.keymap.set_mode 'i'
  web.view.run_js([[ _nullbrowser.hints.currentMatch?.elem?.focus?.(); ]])
end

function hints.action.copy_link()
  web.view.run_js([[_nullbrowser.hints.currentMatch.elem?.href]], {
    on_result = function(url)
      local pipe = io.popen('xclip -selection clipboard', 'w')
      if not pipe then return end
      pipe:write(url)
      pipe:close()
    end,
  })
end

return hints
