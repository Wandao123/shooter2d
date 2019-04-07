function StartStage()
	for i = 1, 5 do  -- 5体生成する。
		StartCoroutine('smallBlueEnemy')
		for j = 1, 15 do  -- 15フレーム待つ。
			coroutine.yield()
		end
	end
end

function smallBlueEnemy()
	local speed = 2.0
	local angle = math.pi / 2
	local enemy = GenerateMover('SmallBlueEnemy', ScreenWidth / 4, 0, speed, angle)
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
