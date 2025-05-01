local t = {}

local indent = 0

local function with_indent(func)
  indent = indent + 1
  func()
  indent = indent - 1
end

function t.describe(description, func)
  local prefix = string.rep(' ', indent)
  print(prefix .. '\x1b[1m' .. description .. '\x1b[0m')
  io.flush()

  with_indent(func)
end

function t.context(description, func)
  local prefix = string.rep(' ', indent)
  print(prefix .. '│ \x1b[32m' .. description .. '\x1b[0m')
  io.flush()
  with_indent(func)
end

function t.it(description, func)
  local prefix = string.rep(' ', indent - 1)
  print(prefix .. '└─⚪\x1b[36m' .. description .. '\x1b[0m')
  io.flush()
  with_indent(func)
end

---@diagnostic disable-next-line: unused-local
function t.xit(description, func)
  print('└─⚪\x1b[33m' .. description .. '\x1b[0m')
  io.flush()
end

function t.expect(received)
  local assert = {}

  local function assert_error(messages)
    print('❌AssertionError:')
    for _, msg in ipairs(messages) do
      print('  ' .. msg)
    end
    error('assertion_error')
  end

  function assert.to_equal(expected)
    if web.utils.equals(received, expected) then return end
    assert_error({ 'Expected: ' .. web.inspect(expected), 'Received: ' .. web.inspect(received) })
  end

  function assert.to_be_true()
    if received == true then return end
    assert_error({ 'Expected: ' .. web.inspect(received) .. ' to be true' })
  end

  function assert.to_be_false()
    if received == false then return end
    assert_error({ 'Expected: ' .. web.inspect(received) .. ' to be false' })
  end

  return assert
end

_G.describe = t.describe
_G.context = t.context
_G.it = t.it
_G.xit = t.xit
_G.expect = t.expect

return t
