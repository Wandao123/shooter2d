local parameters = {
	InvincibleFrames = 360,
	InputDelayFrames = 90,
	ID = PlayerID.Reimu,
	HighSpeed = 4.5,
	LowSpeed = 2.0,
	NormalShot = ShotID.ReimuNormal,
	ShotDelayFrames = 6,
	BulletSpeed = 30.0
}

local player = nil

-- �������B
local function Initialize()
	player = GeneratePlayer(parameters.ID, ScreenWidth * 0.5, ScreenHeight + PlayerHeight - parameters.InputDelayFrames)
	player:TurnInvincible(parameters.InvincibleFrames / 2)
	coroutine.yield()
end

-- ���@�̕��A�����B
local function Rebirth()
	if (not player:IsEnabled()) and (player.Life > 0) then
		player.PosX = ScreenWidth * 0.5
		player.PosY = ScreenHeight + PlayerHeight
		player:Spawned()
		player:TurnInvincible(parameters.InvincibleFrames);
		coroutine.yield()
		for i = 1, parameters.InputDelayFrames do
			-- ������SetVelocity���g���ƁA�ړ����������̂Ƃ���ŕs���������B
			GetPlayer().PosY = GetPlayer().PosY - 1.0
			coroutine.yield()
		end
	end
end

-- ���@�̈ړ��B���A�Ƃ̌��ˍ�������APlayer�N���X���ŏ����ł��Ȃ��B
local function Move()
	while true do
		-- TODO: Player�N���X����LowSpeed/HighSpeed���g���悤�ɏC���BSetVelocity�̈����� (direction: Vector2, slowMode: bool) �ɕύX�B
		local speed = GetKey(CommandID.Slow) and parameters.LowSpeed or parameters.HighSpeed
		local velX, velY = 0.0, 0.0
		if GetKey(CommandID.Leftward) then
			velX = -speed
		end
		if GetKey(CommandID.Rightward) then
			velX = speed
		end
		if GetKey(CommandID.Forward) then
			velY = -speed
		end
		if GetKey(CommandID.Backward) then
			velY = speed
		end
		-- TODO: Vector2�N���X�ŏ�����悤�ɂ���B
		if velX ~= 0.0 or velY ~= 0.0 then
			velX = velX * speed / math.sqrt(velX ^ 2 + velY ^ 2)
			velY = velY * speed / math.sqrt(velX ^ 2 + velY ^ 2)
		end
		player:SetVelocity(velX, velY)
		coroutine.yield()
	end
end

-- ���@�̃V���b�g�B
local function Shoot()
	while true do
		if GetKey(CommandID.Shot) then
			GenerateShot(parameters.NormalShot, player.PosX - 12, player.PosY, parameters.BulletSpeed, -math.pi / 2)
			GenerateShot(parameters.NormalShot, player.PosX + 12, player.PosY, parameters.BulletSpeed, -math.pi / 2)
			for i = 1, parameters.ShotDelayFrames do
				coroutine.yield()
			end
		else	
			coroutine.yield()
		end
	end
end

function PlayerScript()
	Initialize()
	local co = { coroutine.create(Shoot), coroutine.create(Move) }
	repeat
		Rebirth()
		for i = 1, #co do
			coroutine.resume(co[i])
		end
		coroutine.yield()
	until false
end