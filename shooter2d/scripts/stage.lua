function StartStage()
	local co = {}
	for i = 1, 5 do
		co[i] = coroutine.create(smallBlueEnemy)
		for j = 1, 15 do           -- 15フレーム待つ。
			for k = 1, #co do  -- 他の敵を生成している間も更新。
				coroutine.resume(co[k])
			end
			coroutine.yield()
		end
	end
	while true do
		for i = 1, #co do
			if coroutine.status(co[i]) ~= 'dead' then
				coroutine.resume(co[i])
			end
		end
		coroutine.yield()
	end
end

function smallBlueEnemy()
	local speed = 2.0
	local angle = math.pi / 2
	local enemy = GenerateEnemy('SmallBlue', ScreenWidth / 4, 0, speed, angle)
	for i = 1, 120 do
		coroutine.yield()
	end
	local radius = 30.0
	local diffAngle = math.acos(1.0 - 0.5 * speed ^ 2 / radius ^ 2)
	angle = angle - diffAngle / 2
	repeat
		enemy:SetAngle(angle)
		angle = angle - diffAngle
		coroutine.yield()
	until angle < 0.0
end
