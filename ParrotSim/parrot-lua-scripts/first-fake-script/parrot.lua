function vectorToString(v)
  return v[0] .. " " .. v[1] .. " " .. v[2];
end

SHOW_TIME = false;

MAX_FORWARD_SPEED = 3.0;
MAX_SIDEWAYS_SPEED = 3.0;
MAX_VERTICAL_SPEED = 1.0;

MAX_ANGULAR_SPEED = 0.5;

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

-- Control parameters

setForwardSpeed = 0;
setSidewaysSpeed = 0;
setSpinSpeed = 0;
targetHeight = 2;

flyingMode = MODE_LANDED;
inEmergency = false;

timeIndex = 0.0;


function drone:updatePolePositions()
  local i, dronePos, droneDir, droneRight, transform;
  local fov = 90 * math.pi / 180;

  transform = self:getLocationMatrix();
  dronePos = transform:at(3);
  droneDir = transform:at(0);
  droneDir:normalize();
  droneRight = transform:at(1);
  droneRight:normalize();

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
  local angularSpeed = drone:getAngularSpeed();
  self.markedGyroVector = self.markedGyroVector + angularSpeed * deltaT;
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
    local frontFactor = 1.0;
    local backFactor = 1.0;
    local leftFactor = 1.0;
    local rightFactor = 1.0;

    -- Calculate location information 
    local locationMatrix = self:getLocationMatrix();
    local speedVector = self:getSpeed();
    local angularSpeed = self:getAngularSpeed();


    local flatFrontVector = locationMatrix[0];
    flatFrontVector:set(2, 0);
    flatFrontVector:normalize();

    local flatSideVector = Vector:new(0, 0, 1):cross(flatFrontVector);

    -- Calculate raw speeds
    local forwardSpeed = setForwardSpeed * MAX_FORWARD_SPEED;
    local sidewaysSpeed = setSidewaysSpeed * MAX_SIDEWAYS_SPEED;
    local verticalSpeed = 0;
    if locationMatrix:at(2, 3) < targetHeight then
      verticalSpeed = math.min(MAX_VERTICAL_SPEED, targetHeight - locationMatrix:at(2, 3));
    end
    if locationMatrix:at(2, 3) > targetHeight then
      verticalSpeed = -math.min(MAX_VERTICAL_SPEED, locationMatrix:at(2, 3) - targetHeight);
    end

    -- Set speeds
    speedVector = flatFrontVector * forwardSpeed + flatSideVector * sidewaysSpeed + Vector:new(0, 0, verticalSpeed);
    self:setSpeed(speedVector);

    -- Set rotations
    self:setAngularSpeed(Vector:new(0, 0, setSpinSpeed * 90 * math.pi / 180));

    -- Set forces
    self:setMotorPower(LEFT, FRONT, MOTOR_DEFAULT_FORCE * leftFactor * frontFactor);
    self:setMotorPower(RIGHT, FRONT, MOTOR_DEFAULT_FORCE * rightFactor * frontFactor);
    self:setMotorPower(LEFT, BACK, MOTOR_DEFAULT_FORCE * leftFactor * backFactor);
    self:setMotorPower(RIGHT, BACK, MOTOR_DEFAULT_FORCE * rightFactor * backFactor);
  end

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
  print("Target height: " .. targetHeight);
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

function drone:getRotationSensorComponent(i)
  local oldValue = self.markedGyroVector[i];
  self.markedGyroVector:set(i, 0);
  return oldValue;
end

-- Relative pole positions

function drone:isPoleVisible(poleIndex)
  return self.relativePoleVisibility[poleIndex];
end

function drone:getPoleAngle(poleIndex)
  return self.relativePoleAngles[poleIndex];
end

function drone:getPoleCount()
  return world:getPoleCount();
end

--drone:updatePolePositions();
