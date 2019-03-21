function StartStage()
	local speed = 1
	local enemy = GenerateEnemy('SmallBlue', ScreenWidth / 4, 0, 0, speed)
	for i = 1, 120 do
		coroutine.yield()
	end
	local omega = 0.5
	while math.abs(GetVelocity(enemy)["y"]) >= 1.0e-4 do
		print(GetVelocity(enemy)["x"], GetVelocity(enemy)["y"])
		AddForce(enemy, -omega * GetVelocity(enemy)["y"], omega * GetVelocity(enemy)["x"])
		coroutine.yield()
	end
end
