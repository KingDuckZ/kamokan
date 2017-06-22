function num_to_value (num)
  local retval = ""
  local running = true

  while (running)
  do
    local remainder = num % 26
    retval = string.char(97 + remainder) .. retval
    num = math.floor(num / 26)
    running = (num ~= 0)
  end
  return retval
end

paste_counter = KEYS[1]
next_id = redis.call("INCR", paste_counter) - 1
token = num_to_token(next_id)
text = ARGV[1]
ttl = ARGV[2]
lang = ARGV[3]
selfdestruct = ARGV[4]
redis.call("HMSET", token,
	"pastie", text,
	"max_ttl", ttl,
	"lang", lang,
	"selfdes", selfdestruct
)

return token
