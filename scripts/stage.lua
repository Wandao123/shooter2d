function StartStage()
	local speed = 2.0
	local enemy = GenerateEnemy('SmallBlue', ScreenWidth / 4, 0, 0, speed)
	for i = 1, 120 do
		coroutine.yield()
	end
	local radius = 10.0
	while true do
		local velocity = GetVelocity(enemy)
		speed = -math.sqrt(velocity["x"] ^ 2 + velocity["y"] ^ 2)
		local force = { x = -velocity["y"] * speed / radius, y = velocity["x"] * speed / radius }
		AddForce(enemy, force["x"], force["y"])
		--io.write(velocity["x"] + force["x"], " ", velocity["y"] + force["y"])
		--SetVelocity(enemy, velocity["x"] + force["x"], velocity["y"] + force["y"])
		--io.write(" --> ", GetVelocity(enemy)["x"], " ", GetVelocity(enemy)["y"], "\n")
		coroutine.yield()
	end
end
