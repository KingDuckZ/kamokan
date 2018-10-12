local function num_to_token (num)
  local retval = ""
  local running = true
  num = num + 1

  while (running)
  do
    num = num - 1
    local remainder = num % 26
    retval = string.char(97 + remainder) .. retval
    num = math.floor(num / 26)
    running = (num ~= 0)
  end
  return retval
end

local paste_counter_token = KEYS[1]
local flooding_token = KEYS[2]

local flooding_result = redis.call("GET", flooding_token)
if flooding_result then
  return redis.error_reply("UserFlooding")
end

local token_prefix = ARGV[1]
local text = ARGV[2]
local ttl = ARGV[3]
local lang = ARGV[4]
local selfdestruct = ARGV[5]
local flood_wait = ARGV[6]

local next_id = redis.call("INCR", paste_counter_token) - 1
local token = num_to_token(next_id)
local saved = redis.call("HMSET", token_prefix .. token,
  "pastie", text,
  "max_ttl", ttl,
  "lang", lang,
  "selfdes", selfdestruct
)
if saved then
  redis.call("EXPIRE", token_prefix .. token, ttl)
  redis.call("SET", flooding_token, "")
  redis.call("EXPIRE", flooding_token, flood_wait)
else
  return redis.error_reply("PastieNotSaved")
end

return token
