function vectorToString(v)
  return v[0] .. " " .. v[1] .. " " .. v[2];
end

function isNan(x)
  return x ~= x;
end

function isInf(x)
  return (x == -1/0) or (x == 1/0);
end

function isValidNumber(x)
  return (not isInf(x)) and (not isNan(x));
end

function isValidVector(v)
  return isValidNumber(v[0]) and isValidNumber(v[1]) and isValidNumber(v[2]);
end

SHOW_TIME = false;

MAX_FORWARD_SPEED = 3.0;
MAX_SIDEWAYS_SPEED = 3.0;
MAX_VERTICAL_SPEED = 1.0;

MAX_PITCH_SPEED = 45.0 * math.pi / 180;
MAX_ROLL_SPEED = MAX_PITCH_SPEED;
MAX_ANGLE = 20 * math.pi / 180;

MAX_HEIGHT = 10.0;
MIN_HEIGHT = 0.5;

MODE_DISCONNECTED = 0;
MODE_EMERGENCY = 1;
MODE_LANDED = 2;
MODE_FLYING = 3;

-- Sensor parameters

drone.relativePoleVisibility = {};
drone.relativePoleAngles = {};

drone.markedGyroVector = Vector:new();
drone.groundSpeed = Vector:new();

-- Control parameters

setForwardSpeed = 0;
setSidewaysSpeed = 0;
setSpinSpeed = 0;
targetHeight = 2;

flyingMode = MODE_LANDED;
inEmergency = false;

timeIndex = 0.0;


function drone:updatePolePositions()
  local fov = 90 * math.pi / 180;

  local transform = self:getLocationMatrix();
  local dronePos = transform:at(3);
  local droneDir = transform:at(0);
  droneDir:normalize();
  local droneRight = transform:at(1);
  droneRight:normalize();

  local i;
  for i = 0, self:getPoleCount() - 1 do
    local relPolePos = world:getPole(i):getPosition() - dronePos;
    relPolePos:set(2, 0);
    relPolePos:normalize();

    local projection = relPolePos * droneDir;
    if (projection > 0) and (math.acos(projection) < fov / 2) then
      self.relativePoleVisibility[i] = true;

      self.relativePoleAngles[i] = math.acos(projection);
      if (droneRight * relPolePos < 0) then
        self.relativePoleAngles[i] = -self.relativePoleAngles[i];
      end
    else
      self.relativePoleVisibility[i] = false;
    end
  end

  return;
end

function drone:stepSensorData(deltaT)
  self:updatePolePositions();

  -- Gyro information
  local angularSpeed = self:getAngularSpeed();
  self.markedGyroVector = self.markedGyroVector + angularSpeed * deltaT;

  -- Ground speeds
  local locationMatrix = self:getLocationMatrix();
  local speedVector = self:getSpeed();

  local frontVector = locationMatrix[0];
  local sideVector = locationMatrix[1];

  local flatFrontVector = locationMatrix[0];
  flatFrontVector:set(2, 0);
  flatFrontVector:normalize();

  local flatSideVector = locationMatrix[1];
  flatSideVector:set(2, 0);
  flatSideVector:normalize();
  
  local frontSpeed = 0;
  if isValidVector(flatFrontVector) then
    frontSpeed = flatFrontVector * speedVector;
  end

  local sideSpeed = 0;
  if isValidVector(flatSideVector) then
    sideSpeed = flatSideVector * speedVector;
  end
  
  self.groundSpeed:set(0, frontSpeed);
  self.groundSpeed:set(1, sideSpeed);
end

function drone:step(deltaT) 
  timeIndex = timeIndex + deltaT;
  if SHOW_TIME then
    print("TimeIndex: " .. timeIndex .. " sec");
  end

  self:stepSensorData(deltaT);

  if (flyingMode == MODE_EMERGENCY) or (flyingMode == MODE_DISCONNECTED) or (flyingMode == MODE_LANDED) then
    self:setMotorPower(LEFT, FRONT, 0);
    self:setMotorPower(RIGHT, FRONT, 0);
    self:setMotorPower(LEFT, BACK, 0);
    self:setMotorPower(RIGHT, BACK, 0);
  end

  if flyingMode == MODE_FLYING then
    local accelerationVector = (self:getSpeed() - self.lastSpeedVector) * (1 / deltaT);
    if deltaT <= 0 then
      accelerationVector = Vector:new(0, 0, 0);
    end

    local frontFactor = 1.0;
    local backFactor = 1.0;
    local leftFactor = 1.0;
    local rightFactor = 1.0;

    -- Calculate location information 
    local locationMatrix = self:getLocationMatrix();
    local speedVector = self:getSpeed();
    local angularSpeed = self:getAngularSpeed();


    local frontVector = locationMatrix[0];
    local sideVector = locationMatrix[1];

    local flatFrontVector = locationMatrix[0];
    flatFrontVector:set(2, 0);
    flatFrontVector:normalize();

    local flatSideVector = locationMatrix[1];
    flatSideVector:set(2, 0);
    flatSideVector:normalize();
    
    local currentPitch = math.acos(flatFrontVector * frontVector);
    if frontVector[1] < 0 then
      currentPitch = -currentPitch;
    end
    local currentRoll = math.acos(flatSideVector * sideVector);
    if sideVector[1] < 0 then
      currentRoll = -currentRoll;
    end
    
    local currentForwardSpeed = flatFrontVector * speedVector;
    local currentSideSpeed = flatSideVector * speedVector;

    -- Calculate raw speeds
    local forwardSpeed = setForwardSpeed * MAX_FORWARD_SPEED;
    local sidewaysSpeed = setSidewaysSpeed * MAX_SIDEWAYS_SPEED;
    
    -- Calculate target vertical speed
    local verticalSpeed = 0;
    if locationMatrix:at(2, 3) < targetHeight then
      verticalSpeed = math.min(MAX_VERTICAL_SPEED, targetHeight - locationMatrix:at(2, 3));
    end
    if locationMatrix:at(2, 3) > targetHeight then
      verticalSpeed = -math.min(MAX_VERTICAL_SPEED, locationMatrix:at(2, 3) - targetHeight);
    end

    -- Calculate rotations
    local PITCH_STRENGTH = 10.0;
    local PITCH_ACC_CORRECTION_FACTOR = 1.0;
    local targetPitch = 0;
    if isValidNumber(currentForwardSpeed) then
      targetPitch = math.max(-MAX_ANGLE, math.min(MAX_ANGLE, PITCH_STRENGTH * (forwardSpeed - currentForwardSpeed - PITCH_ACC_CORRECTION_FACTOR * (flatFrontVector * accelerationVector))));
    end

    local ROLL_STRENGTH = PITCH_STRENGTH;
    local ROLL_ACC_CORRECTION_FACTOR = PITCH_ACC_CORRECTION_FACTOR;
    local targetRoll = 0;
    if isValidNumber(currentSideSpeed) then
      targetRoll = math.max(-MAX_ANGLE, math.min(MAX_ANGLE, ROLL_STRENGTH * (sidewaysSpeed - currentSideSpeed - ROLL_ACC_CORRECTION_FACTOR * (flatSideVector * accelerationVector))));
    end

    -- Set speeds (dampened mixture with original speed vector)

--    local speedMixture = 0.10; -- After 1 second the old speeds still determines (at most) 10% of the resulting speed
--    speedVector = flatFrontVector * forwardSpeed + flatSideVector * sidewaysSpeed + Vector:new(0, 0, verticalSpeed);
--    speedVector = speedVector * (1 - math.pow(speedMixture, deltaT)) + self:getSpeed() * math.pow(speedMixture, deltaT);

    speedVector = Vector:new(speedVector[0], speedVector[1], verticalSpeed);
    self:setSpeed(speedVector);

    -- Set rotation speeds
    local angularSpeed = Vector:new();
	tolua.takeownership(angularSpeed); -- Ditto... same as before
    if isValidVector(flatSideVector) and isValidNumber(currentPitch) then
      angularSpeed = angularSpeed + (flatSideVector * (targetPitch - currentPitch));
    end
    if isValidVector(frontVector) and isValidNumber(currentRoll) then
      angularSpeed = angularSpeed + (frontVector * -(targetRoll - currentRoll));
    end
    angularSpeed:set(2, angularSpeed[2] + setSpinSpeed * 90 * math.pi / 180);
    self:setAngularSpeed(angularSpeed);


    -- Set forces
    self:setMotorPower(LEFT, FRONT, MOTOR_DEFAULT_FORCE * leftFactor * frontFactor);
    self:setMotorPower(RIGHT, FRONT, MOTOR_DEFAULT_FORCE * rightFactor * frontFactor);
    self:setMotorPower(LEFT, BACK, MOTOR_DEFAULT_FORCE * leftFactor * backFactor);
    self:setMotorPower(RIGHT, BACK, MOTOR_DEFAULT_FORCE * rightFactor * backFactor);
  end

  self.lastSpeedVector = self:getSpeed();

-- Place drone stabilization code in here!

-- Called during every simulation cycle with a time-step of deltaT
end

function drone:flyForward(speed)
  setForwardSpeed = math.min(1, math.max(-1, speed));
end

function drone:flySideways(speed)
  setSidewaysSpeed = math.min(1, math.max(-1, speed));
end

function drone:spin(speed)
  setSpinSpeed = math.min(1, math.max(-1, speed));
end

function drone:setHeight(height)
  targetHeight = MIN_HEIGHT + math.min(1.0, math.max(0.0, height)) * (MAX_HEIGHT - MIN_HEIGHT);
end

function drone:hover()
  setForwardSpeed = 0;
  setSidewaysSpeed = 0;
  setSpinSpeed = 0;
end

function drone:getFlyingState()
  return flyingMode;
end

function drone:land()
  if flyingMode == MODE_FLYING then
    flyingMode = MODE_LANDED;
  end
end

function drone:takeoff() 
  if flyingMode == MODE_LANDED then 
    flyingMode = MODE_FLYING;
    self:hover();
    targetHeight = 1.0;
  end
end

function drone:setEmergency(e)
  if e then
    flyingMode = MODE_EMERGENCY;
    inEmergency = true;
  else
    if flyingMode == MODE_EMERGENCY then
      flyingMode = MODE_LANDED;
      inEmergency = false;
    end
  end
end

-- Sensory data

function drone:getRotationSensorData()
  local gyroResult = self.markedGyroVector;
  self.markedGyroVector = Vector:new();
  return gyroResult[0], gyroResult[1], gyroResult[2];
end

function drone:getGroundSpeed()
  return self.groundSpeed[0], self.groundSpeed[1], self.groundSpeed[2];
end

-- Relative pole positions

function drone:isPoleVisible(poleIndex)
  if self.relativePoleVisibility[poleIndex] then
    --print("Pole " .. poleIndex .. " visible");
  else
    --print("Pole " .. poleIndex .. " invisible");
  end
  return self.relativePoleVisibility[poleIndex];
end

function drone:getPoleAngle(poleIndex)
  return self.relativePoleAngles[poleIndex];
end

function drone:getPoleCount()
  return world:getPoleCount();
end

-- Extended information

function drone:javaGetPosition() 
  local pos = self:getPosition();
  return pos[0], pos[1], pos[2];
end

function drone:setPositionAndResetOrientation(x, y, z)
 self:setLocationMatrix(Matrix:new());
 self:setPosition(Vector:new(x, y, z));
 drone:setSpeed(Vector:new());
 drone:setAngularVelocity(Vector:new()); 
end

--drone:updatePolePositions();
drone.lastSpeedVector = Vector:new(0, 0, 0);
