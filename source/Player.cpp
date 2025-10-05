#include "Player.h"

namespace BlastOff
{
	PlayerConfig::PlayerConfig(const float targetFramerate) :
		m_SpaceshipHeight(1),
		m_FlameFramerate(30),
		m_ThrustAccelerationMultiplier(10),
		m_TerminalVelocity(20),
		m_GroundedThreshold(1),
		m_SpeedupMultiplier(3 / 2.0f),
		m_RunningOnFumesThreshold(2),
		m_RefuelAmount(2 / 3.0f)
	{
		m_RegularSmoothingFrames = (int)roundf(10 * targetFramerate / 60);
		m_RotationSmoothingFrames = (int)roundf(5 * targetFramerate / 60);
	}

	float PlayerConfig::GetSpaceshipHeight() const 
	{ 
		return m_SpaceshipHeight; 
	}

	float PlayerConfig::GetFlameFramerate() const
	{
		return m_FlameFramerate;
	}

	float PlayerConfig::GetThrustAccelerationMultiplier() const 
	{ 
		return m_ThrustAccelerationMultiplier; 
	}

	float PlayerConfig::GetTerminalVelocity() const 
	{ 
		return m_TerminalVelocity; 
	}

	int PlayerConfig::GetRegularSmoothingFrames() const 
	{ 
		return m_RegularSmoothingFrames; 
	}

	int PlayerConfig::GetRotationSmoothingFrames() const
	{ 
		return m_RotationSmoothingFrames; 
	}
	
	float PlayerConfig::GetGroundedThreshold() const 
	{ 
		return m_GroundedThreshold; 
	}

	float PlayerConfig::GetSpeedupMultiplier() const 
	{ 
		return m_SpeedupMultiplier; 
	}

	float PlayerConfig::GetRunningOnFumesThreshold() const 
	{ 
		return m_RunningOnFumesThreshold; 
	}

	float PlayerConfig::GetRefuelAmount() const 
	{ 
		return m_RefuelAmount; 
	}


	Player::Player(
		const GameOutcome* const gameOutcome,
		const Rect2f* const worldBounds,
		const Platform* const platform, 
		const CoordinateTransformer* const coordTransformer,
		const GameConstants* const gameConstants,
		const ProgramConfiguration* const programConfig,
		const InputManager* const inputManager,
		ImageTextureLoader* const imageTextureLoader
	) :
		m_MaximumFuel(10),
		m_MaxSpeedupTick(5),
		m_GameOutcome(gameOutcome),
		m_WorldBounds(worldBounds),
		m_Platform(platform),
		m_CoordTransformer(coordTransformer),
		m_GameConstants(gameConstants),
		m_ProgramConfig(programConfig),
		m_InputManager(inputManager)
	{
		const auto initializeConfig =
			[&, this]()
			{
				const float targetFramerate = 
				{
					(float)m_ProgramConfig->GetTargetFramerate()
				};
				m_Config = 
				{
					std::make_unique<PlayerConfig>(targetFramerate)
				};
			};

		const auto initializeFuel =
			[this]()
			{
				m_CurrentFuel = m_MaximumFuel;
			};

		const auto initializeSpaceship =
			[&, this]()
			{
				m_Spaceship = ImageSprite::LoadFromPath(
					c_SpaceshipTexturePath,
					coordTransformer,
					programConfig,
					imageTextureLoader
				);

				const Vector2i imageSize = m_Spaceship->GetImageSize();
				const float aspectRatio = imageSize.x / (float)imageSize.y;

				const Vector2f viewportSize = 
				{
					m_CoordTransformer->GetViewportSize()
				};
				const float spaceshipHeight = m_Config->GetSpaceshipHeight();

				const Vector2f platformSize = m_Platform->GetEngineSize();
				const float platformHeight = platformSize.y;

				const Vector2f vpSize = viewportSize;
				const float shipH = spaceshipHeight;
				const float platH = platformHeight;

				const Rect2f engineRect(
					{ 0, platH + ((shipH - vpSize.y) / 2.0f) },
					{ spaceshipHeight * aspectRatio, spaceshipHeight }
				);
				m_Spaceship->SetEngineRect(engineRect);
			};

		const auto initializeSpaceshipFlame =
			[&, this]()
			{
				// where did i get these numbers? lol
				const Rect2f engineRect(
					{ 0, -0.35415217f },
					{ 0.39959502f, 0.27455562f }
				);
				m_SpaceshipFlame = ImageSprite::LoadFromPath(
					engineRect,
					c_FlameTexturePath,
					coordTransformer,
					programConfig,
					imageTextureLoader
				);
				m_SpaceshipFlame->SetParent(m_Spaceship.get());
			};

		initializeConfig();
		initializeFuel();
		initializeSpaceship();
		initializeSpaceshipFlame();
	}

	bool Player::IsStationary() const
	{
		return m_StationaryFrameCount > 2;
	}

	bool Player::IsOutOfFuel() const
	{
		return m_CurrentFuel <= 0;
	}

	float Player::GetFuelRatio() const
	{
		const float unbounded = m_CurrentFuel / m_MaximumFuel;
		return fmaxf(unbounded, 0);
	}

	float Player::GetSpeedUpRatio() const
	{
		const float unbounded = m_SpeedupTick / m_MaxSpeedupTick;
		return fmaxf(unbounded, 0);
	}

	void Player::Update()
	{
		const auto calculateMouseOffset =
			[this]() -> optional<Vector2f>
			{
				const Vector2f mousePosition =
				{
					m_InputManager->CalculateMousePosition()
				};
				const Vector2f enginePosition =
				{
					m_Spaceship->CalculateRealPosition()
				};
				const Vector2f result = mousePosition - enginePosition;
				return result;
			};

		const auto calculateThrustAcceleration =
			[&, this]() -> Vector2f
			{
				const optional<Vector2f> mouseOffset = calculateMouseOffset();

				const Vector2f normalizedOffset = mouseOffset->Normalize();
				const float multiplier =
				{
					m_Config->GetThrustAccelerationMultiplier()
				};
				const Vector2f result =
				{
					normalizedOffset * multiplier
				};

				if (result.y < 0 && IsGrounded())
					return Vector2f::Zero();
				else
					return result;
			};

		const auto updateThrustAcceleration =
			[&, this]()
			{
				const bool spaceNotPressed =
				{
					!m_InputManager->GetKeyDown(KEY_SPACE)
				};
				const bool fuelIsEmpty = IsOutOfFuel();
				const bool ignoreInput =
				{
					(*m_GameOutcome) != GameOutcome::None
				};

				if (spaceNotPressed || fuelIsEmpty || ignoreInput)
					m_ThrustAcceleration = Vector2f::Zero();
				else
 					m_ThrustAcceleration = calculateThrustAcceleration();
			};

		const auto updateFuel =
			[this]()
			{
				if (IsThrusting())
				{
					const float targetFrametime = 
					{
						m_ProgramConfig->GetTargetFrametime()
					};
					m_CurrentFuel -= targetFrametime;
				}
			};

		const auto applySpeedup =
			[this]()
			{
				if (IsSpedUp() && IsThrusting())
				{
					m_ThrustAcceleration *= m_Config->GetSpeedupMultiplier();

					const float frametime =
					{
						m_ProgramConfig->GetTargetFrametime()
					};
					m_SpeedupTick -= frametime;
				}
			};

		const auto applyRunningOnFumes =
			[this]()
			{
				if (IsRunningOnFumes())
					m_ThrustAcceleration *= GetRandomFloat();
			};

		const auto applyThrustAcceleration =
			[this]()
			{
				if (IsThrusting())
				{
					const float frametime =
					{
						m_ProgramConfig->GetTargetFrametime()
					};
					m_Velocity += m_ThrustAcceleration * frametime;
				}
			};

		const auto applyGravity =
			[this]()
			{
				const float acceleration =
				{
					m_GameConstants->GetGravitationalAcceleration()
				};
				const Vector2f gravityVector = Vector2f::Down() * acceleration;

				const float frametime = m_ProgramConfig->GetTargetFrametime();
				m_Velocity += gravityVector * frametime;
			};

		const auto limitVelocity =
			[this]()
			{
				if (IsGoingTooFast())
				{
					const float terminalVelocity =
					{
						m_Config->GetTerminalVelocity()
					};
					m_Velocity =
					{
						m_Velocity.Normalize() * terminalVelocity
					};
				}
			};

		const auto calculateWorldEdge =
			[this](const Direction side) -> Edge2f
			{
				const optional<Edge2f> result = m_WorldBounds->GetEdge(side);
				if (!result)
				{
					const char* const message =
					{
						"Rect2f::GetEdge(const Direction side) failed."
					};
					throw std::runtime_error(message);
				}
				return *result;
			};

		const auto applyGroundedThreshold =
			[this]()
			{
				if (IsGrounded())
					m_Velocity = Vector2f::Zero();
			};

		const auto applyCollisionMultiplier =
			[this]()
			{
				const float collisionMultiplier =
				{
					m_GameConstants->GetCollisionMultiplier()
				};
				m_Velocity *= collisionMultiplier;
			};

		const auto applyBoundXCollision =
			[&, this](const Edge2f edge)
			{
				m_DidCollideHorizontally = true;

				m_Velocity = m_Velocity.InvertX();
				applyCollisionMultiplier();

				const Vector2f previousPosition =
				{
					m_Spaceship->CalculateRealPosition()
				};
				const Vector2f newPosition(edge.position, previousPosition.y);
				m_Spaceship->SetLocalPosition(newPosition);
			};

		const auto calculatePropCollisionPosition =
			[this](const Edge2f edge) -> float
			{
				const Vector2f size = m_Spaceship->GetEngineSize();
				switch (edge.side)
				{
				case Direction::Up:
					return edge.position + (size.y / 2.0f);

				case Direction::Down:
					return edge.position - (size.y / 2.0f);

				case Direction::Left:
					return edge.position - (size.x / 2.0f);

				case Direction::Right:
					return edge.position + (size.x / 2.0f);

				default:
					const char* const message =
					{
						"Player::Update(): "
						"invalid value of Edge2f parameter: "
						"invalid member \"side\"."
					};
					throw std::runtime_error(message);
				}
			};

		const auto applyPropXCollision =
			[&, this](const Edge2f edge)
			{
				m_DidCollideHorizontally = true;

				m_Velocity = m_Velocity.InvertX();
				applyCollisionMultiplier();

				const Vector2f previousPosition =
				{
					m_Spaceship->CalculateRealPosition()
				};
				const float offsettedPosition =
				{
					calculatePropCollisionPosition(edge)
				};
				const Vector2f newPosition(
					offsettedPosition,
					previousPosition.y
				);
				m_Spaceship->SetLocalPosition(newPosition);
			};

		const auto applyVelocitySign = 
			[this](const float velocitySign)
			{
				if (m_Velocity.y > 0 && velocitySign < 0)
					m_Velocity = m_Velocity.InvertY();

				else if (m_Velocity.y < 0 && velocitySign > 0)
					m_Velocity = m_Velocity.InvertY();
			};

		const auto applyPropYCollision =
			[&, this](const Edge2f edge)
			{
				const optional<float> velocitySign = edge.GetSign();
				if (velocitySign)
					applyVelocitySign(*velocitySign);

				applyCollisionMultiplier();

				if (edge.side == Direction::Up)
				{
					applyGroundedThreshold();
					m_BottomCollision = true;
				}

				const Vector2f previousPosition =
				{
					m_Spaceship->CalculateRealPosition()
				};
				const float offsettedPosition =
				{
					calculatePropCollisionPosition(edge)
				};
				const Vector2f newPosition(
					previousPosition.x,
					offsettedPosition
				);
				m_Spaceship->SetLocalPosition(newPosition);
			};

		const auto applyWorldBoundCollisions =
			[&, this]()
			{
				const Vector2f position = m_Spaceship->CalculateRealPosition();

				const Edge2f leftEdge = calculateWorldEdge(Direction::Left);
				if (position.CollideWithEdge(leftEdge))
				{
					applyCollisionMultiplier();
					applyBoundXCollision(leftEdge);
					return;
				}

				const Edge2f rightEdge = calculateWorldEdge(Direction::Right);
				if (position.CollideWithEdge(rightEdge))
				{
					applyCollisionMultiplier();
					applyBoundXCollision(rightEdge);
					return;
				}
			};

		const auto applyTopLineCollision =
			[&, this](
				const Line2f topLine,
				const Line2f leftLine,
				const Line2f rightLine
			)
			{
				const Vector2f realPosition =
				{
					m_Spaceship->CalculateRealPosition()
				};
				const float topDistance = topLine.Distance(realPosition);
				const float leftDistance = leftLine.Distance(realPosition);
				const float rightDistance = rightLine.Distance(realPosition);

				if (topDistance > leftDistance)
				{
					const Edge2f edge =
					{
						Direction::Left,
						leftLine.AverageX()
					};
					applyPropXCollision(edge);
				}
				else if (topDistance > rightDistance)
				{
					const Edge2f edge =
					{
						Direction::Right,
						rightLine.AverageX()
					};
					applyPropXCollision(edge);
				}
				else
				{
					const Edge2f edge = { Direction::Up, topLine.AverageY() };
					applyPropYCollision(edge);
				}
			};

		const auto applyPlatformCollisions =
			[&, this]()
			{
				const Line2f topLine = m_Platform->GetTopCollisionLine();
				const Line2f leftLine = m_Platform->GetLeftCollisionLine();
				const Line2f rightLine = m_Platform->GetRightCollisionLine();

				m_BottomCollision = false;

				if (m_Spaceship->CollideWithLine(topLine))
					applyTopLineCollision(topLine, leftLine, rightLine);
				else if (m_Spaceship->CollideWithLine(leftLine))
				{
					const Edge2f edge =
					{
						Direction::Left,
						leftLine.AverageX()
					};
					applyPropXCollision(edge);
				}
				else if (m_Spaceship->CollideWithLine(rightLine))
				{
					const Edge2f edge =
					{
						Direction::Right,
						rightLine.AverageX()
					};
					applyPropXCollision(edge);
				}
			};

		const auto calculateVelocityForRotation =
			[this]()
			{
				if (!m_DidCollideHorizontally)
					return m_Velocity;
				else
				{
					m_DidCollideHorizontally = false;
					return m_Velocity.InvertX();
				}
			};

		const auto applyRotation =
			[&, this]()
			{
				const Vector2f velocityToUse = calculateVelocityForRotation();
				const auto angleInRadians = velocityToUse.Angle();

				if (angleInRadians)
					m_Spaceship->SetLocalRotation(*angleInRadians);
				else
					m_Spaceship->SetLocalRotation(0);
			};

		const auto fadeSpaceshipFlame =
			[this]()
			{
				if (IsThrusting())
				{
					const int programFramerate = 
					{
						m_ProgramConfig->GetTargetFramerate()
					};
					const float animationFramerate = 
					{
						m_Config->GetFlameFramerate()
					};

					const int64_t framesBetweenUpdates = 
					{
						(int64_t)roundf(programFramerate / animationFramerate)
					};

					if (m_FramesThrusted % framesBetweenUpdates == 0)
					{
						const float newOpacity = GetRandomFloat();
						m_SpaceshipFlame->SetOpacity(newOpacity);
					}
					m_FramesThrusted++;
				}
			};

		const auto updateStationaryFrames =
			[this]()
			{
				if (m_BottomCollision)
					m_StationaryFrameCount++;
				else
					m_StationaryFrameCount = 0;
			};

		const auto applyVelocity =
			[this]()
			{
				const float frametime = m_ProgramConfig->GetTargetFrametime();
				m_Spaceship->Move(m_Velocity * frametime);
			};

		if (!m_IsFrozen)
		{
			applyWorldBoundCollisions();
			applyPlatformCollisions();
			applyRotation();
			updateThrustAcceleration();
			updateFuel();
			applyRunningOnFumes();
			applySpeedup();
			applyThrustAcceleration();
			applyGravity();
			updateStationaryFrames();
			limitVelocity();
			applyVelocity();
		}
		fadeSpaceshipFlame();
	}

	void Player::Draw() const
	{
		if (IsThrusting())
			m_SpaceshipFlame->Draw();

		m_Spaceship->Draw();
	}

	void Player::Freeze()
	{
		SetFrozen(true);
	}

	void Player::Unfreeze()
	{
		SetFrozen(false);
	}

	void Player::SetFrozen(const bool isFrozen)
	{
		m_IsFrozen = isFrozen;
	}

	void Player::Teleport(const Vector2f position)
	{
		m_Spaceship->SetLocalPosition(position);
	}

	void Player::TeleportToY(const float yPosition)
	{
		const Rect2f engineRect = m_Spaceship->GetEngineRect();
		Teleport({ engineRect.x, yPosition });
	}

	void Player::EmptyFuel()
	{
		m_CurrentFuel = 0;
	}

	void Player::Refuel()
	{
		const float refuelAmount = m_Config->GetRefuelAmount() * m_MaximumFuel;
		m_CurrentFuel += refuelAmount;

		m_CurrentFuel = fminf(m_CurrentFuel, m_MaximumFuel);
	}

	void Player::RefillSpeedup()
	{
		m_SpeedupTick = m_MaxSpeedupTick;
	}

	Rect2f Player::GetEngineRect() const
	{
		return m_Spaceship->GetEngineRect();
	}

	float Player::GetEdgePosition(const Direction side) const
	{
		const Rect2f spaceshipRect = m_Spaceship->GetEngineRect();
		const optional<float> result =
		{
			spaceshipRect.GetEdgePosition(Direction::Up)
		};
		if (!result)
		{
			const string message =
			{
				"Rect2f::GetEdgePosition"
				"(" + DirectionToString(side) + ") failed."
			};
			throw std::runtime_error(message);
		}
		return *result;
	}

	Vector2f Player::GetVelocity() const
	{
		return m_Velocity;
	}

	const char* const Player::c_SpaceshipTexturePath = "Spaceship.png";
	const char* const Player::c_FlameTexturePath = "SpaceshipFlames.png";

	bool Player::IsThrusting() const
	{
		return m_ThrustAcceleration != Vector2f::Zero();
	}

	bool Player::IsGoingTooFast() const
	{
		const float terminalVelocity =
		{
			m_Config->GetTerminalVelocity()
		};
		return m_Velocity.Magnitude() > terminalVelocity;
	}

	bool Player::IsGrounded() const
	{
		const float minVelocity = m_Config->GetGroundedThreshold();

		const bool zeroThrust = m_ThrustAcceleration.y <= 0;
		const bool belowMin = m_Velocity.Magnitude() < minVelocity;

		return zeroThrust && belowMin;
	}

	bool Player::IsRunningOnFumes() const
	{
		const float threshold = m_Config->GetRunningOnFumesThreshold();
		return m_CurrentFuel <= threshold;
	}

	bool Player::IsSpedUp() const
	{
		return m_SpeedupTick > 0;
	}
}
