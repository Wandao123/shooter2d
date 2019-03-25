function StartStage()
	local speed = 2.0
	local angle = math.pi / 2
	local enemy = GenerateEnemy('SmallBlue', ScreenWidth / 4, 0, speed, angle)
	for i = 1, 120 do
		coroutine.yield()
	end

	--[[local numDivision = 20
	local dt = math.pi / 2 / 10
	for i = 0, numDivision do
		SetAngle(enemy, angle - dt * i)
		coroutine.yield()
	end]]

	local radius = 30.0
	local diffAngle = math.acos(1.0 - 0.5 * speed ^ 2 / radius ^ 2)
	angle = angle - diffAngle / 2
	repeat
		SetAngle(enemy, angle)
		angle = angle - diffAngle
		coroutine.yield()
	until angle < 0.0
end
