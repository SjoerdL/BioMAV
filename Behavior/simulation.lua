function createWorld() 
  local worldBlock, pole, tex_wall, tex_floor, tex_ceiling, tex_pole;

  --- Pole
  tex_pole = Texture:new("res/Textures/seamless_pole.jpg");
  pole = Pole:new(Vector:new(5,  0,  0), 0.1, 3, tex_pole);
  world:addWorldElement(pole);

  pole = Pole:new(Vector:new(-5,  0,  0), 0.1, 3, tex_pole);
  world:addWorldElement(pole);

  tex_floor = Texture:new("res/Textures/seamless_blue_floor_lines2.jpg");
  worldBlock = WorldBlock:new(Vector:new(-10, -10, -1), Vector:new(10, 10, 0), tex_floor, 20);
  world:addWorldElement(worldBlock);

  tex_wall = Texture:new("res/Textures/seamless_brick_wall2.jpg");

  worldBlock = WorldBlock:new(Vector:new(-11, -11,  0), Vector:new(-10,  11, 10), tex_wall, 3);
  world:addWorldElement(worldBlock);

  worldBlock = WorldBlock:new(Vector:new( 11, -11,  0), Vector:new( 10,  11, 10), tex_wall, 3);
  world:addWorldElement(worldBlock);

  worldBlock = WorldBlock:new(Vector:new(-10, -11,  0), Vector:new( 10, -10, 10), tex_wall, 3);
  world:addWorldElement(worldBlock);

  worldBlock = WorldBlock:new(Vector:new(-10,  11,  0), Vector:new( 10,  10, 10), tex_wall, 3);
  world:addWorldElement(worldBlock);

  tex_ceiling = Texture:new("res/Textures/seamless_systemceiling.jpg");
  worldBlock = WorldBlock:new(Vector:new(-10, -10, 9), Vector:new(10, 10, 10), tex_ceiling, 5);
  world:addWorldElement(worldBlock);


--  drone:setPosition(Vector:new(0, 0, 3));
--  drone:setSpeed(Vector:new(10, 0, 10));
--  drone:setAngularSpeed(Vector:new(2, 1, 1));
--  drone:takeoff();
--  drone:flySideways(0.0);
--  drone:flyForward(0.5);
--  drone:spin(0.25);
end

timeIndex = 0;

function beforeStep(deltaT)
--  local locMatrix, locPos;
--  locMatrix = drone:getLocationMatrix();
--  locPos = drone:getPosition();

  timeIndex = timeIndex + deltaT;
--  print("Time index: " .. timeIndex);

--  print("Dir: " .. locMatrix[0][0] .. " " .. locMatrix[1][0] .. " " .. locMatrix[2][0]);
--  print("Pos: " .. locPos[0] .. " " .. locPos[1] .. " " .. locPos[2]);
--  drone:addTorque(0, 0, 0.1);
end

createWorld();
