local token = KEYS[1]
local result = redis.call("HMGET", token, "pastie", "selfdes", "lang")
local selfdes = 0
local deleted = 0
if result[2] == 1 then
	deleted = redis.call("DEL", token)
	selfdes = 1
end

return {result[1], selfdes, deleted, result[3]}
