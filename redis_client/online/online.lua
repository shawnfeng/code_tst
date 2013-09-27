local uid = 'U_'..KEYS[1]
local sn = KEYS[2]
local st = KEYS[3]

redis.call('HSET', uid, sn, st)

--redis.call('HMSET', 'S_'..sn, ARGV[1], ARGV[2], ARGV[3], ARGV[4])
local ksn = 'S_'..sn
for i=1, #ARGV, 2 do
   redis.call('HSET', ksn, ARGV[i], ARGV[i+1])
end
--for i, v in ipairs(ARGV) do
--   redis.call('HSET', 'S_'..sn, sn, st)

--end

--return {uid, session}
return 1
--return KEYS

