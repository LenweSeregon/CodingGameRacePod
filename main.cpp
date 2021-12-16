#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "math.h"


// ===================================
// ==== Classes forward declaration
// ===================================

class Pod;
class PodStrategy;

// ===================================
// ==== Utility classes
// ===================================

template <typename T>
class Vector2
{
	static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value || std::is_same<T, int>::value, "Type is not valid");

public:

	T X;
	T Y;

	Vector2<T>() :
		X(0), Y(0)
	{

	}

	Vector2(T aX, T aY) :
		X(aX),
		Y(aY)
	{

	}

	Vector2<T>(Vector2<T> const&) = default;

	Vector2<T>& operator=(Vector2<T> other)
	{
		X = other.X;
		Y = other.Y;
		return *this;
	}

	bool operator==(const Vector2& other) const
	{
		return X == other.X && Y == other.Y;
	}

	Vector2<T> operator+(const Vector2& other) const
	{
		return Vector2<T>(X + other.X, Y + other.Y);
	}
	Vector2<T> operator+=(Vector2& other)
	{
		this = this + other;
	}
	Vector2<T> operator-(const Vector2& other) const
	{
		return Vector2<T>(X - other.X, Y - other.Y);
	}
	Vector2<T> operator*(float k) const
	{
		return Vector2<T>(k*X, k*Y);
	}
	Vector2<T> operator*=(float k)
	{
		this = k * this;
	}

	Vector2<float> Normalize() const
	{
		const float magnitude = sqrt(X*X + Y * Y);
		std::cerr << "Magnitude " << magnitude << " - " << X << " - " << Y << std::endl;
		return Vector2<float>(X / magnitude, Y / magnitude);
	}

	float Dot(const Vector2& other) const
	{
		return X * other.X + Y * other.Y;
	}

	float DistanceSqr(const Vector2& other) const
	{
		return (X - other.X)*(X - other.X) + (Y - other.Y)*(Y - other.Y);
	}
};

class SimulationResult
{
public:

	bool Shield;
	bool Boost;
	int X;
	int Y;
	int Thrust;

	SimulationResult(bool aBoost, bool aShield, int aX, int aY, int aThrust) :
		Boost(aBoost),
		Shield(aShield),
		X(aX),
		Y(aY),
		Thrust(aThrust)
	{

	}
};

class SimulationEntry
{
public:

	int XPod;
	int YPod;
	int XOpponent;
	int YOpponent;
	int XNextCheckpoint;
	int YNextCheckpoint;
	int AngleNextCheckpoint;
	int DistanceNextCheckpoint;
	const Pod& PodOpponent;

	SimulationEntry(int aXPod, int aYPod, int aXOpponent, int aYOpponent,
		int aXNextCheckpoint, int aYNextCheckpoint, int aAngleNextCheckpoint, int aDistanceNextCheckpoint,
		const Pod& aPodOpponent) :
		XPod(aXPod),
		YPod(aYPod),
		XOpponent(aXOpponent),
		YOpponent(aYOpponent),
		XNextCheckpoint(aXNextCheckpoint),
		YNextCheckpoint(aYNextCheckpoint),
		AngleNextCheckpoint(aAngleNextCheckpoint),
		DistanceNextCheckpoint(aDistanceNextCheckpoint),
		PodOpponent(aPodOpponent)
	{

	}
};

class Entity
{

protected:

	Vector2<int> _mPosition;

public:

	Entity(Vector2<int> aPosition) :
		_mPosition(aPosition)
	{

	}

	// -- Getters
	Vector2<int> Position() const { return _mPosition; }
	// -- Setters
	void SetPosition(Vector2<int> aValue) { _mPosition = aValue; }
};


class Checkpoint : public Entity
{
private:

	int _mAngle;
	int _mDistance;
	bool _mBestBoostIndex;
	Vector2<int> _mNextCheckpointPosition;

public:

	Checkpoint(Vector2<int> aPosition, int aAngle, int aDistance) :
		Entity(aPosition),
		_mAngle(aAngle),
		_mDistance(aDistance),
		_mBestBoostIndex(false),
		_mNextCheckpointPosition(Vector2<int>{-1, -1})
	{

	}

	Checkpoint(Checkpoint const&) = default;

	Checkpoint& operator=(Checkpoint other)
	{
		_mAngle = other._mAngle;
		_mDistance = other._mDistance;
		_mBestBoostIndex = other._mBestBoostIndex;
		return *this;
	}

	static float Radius;

	Vector2<int> GetNextCheckpointPosition() const
	{
		return _mNextCheckpointPosition;
	}

	int GetAngle() const
	{
		return _mAngle;
	}

	int GetDistance() const
	{
		return _mDistance;
	}

	bool GetBestBoostIndex() const
	{
		return _mBestBoostIndex;
	}

	void SetNextCheckpointPosition(Vector2<int> aValue)
	{
		_mNextCheckpointPosition = aValue;
	}

	void SetAngle(int aValue)
	{
		_mAngle = aValue;
	}

	void SetDistance(int aValue)
	{
		_mDistance = aValue;
	}

	void SetBestBoostIndex(bool aValue)
	{
		_mBestBoostIndex = aValue;
	}
};

// -------------------------------------------------------------------
// we don't know the checkpoints in advance (they are discovered from the input, one after the other)
// however, since they are repeated several times (laps), we can remember them (and know everything from lap 2 onwards)
// this is done by feeding the current checkpoint to the CheckpointManager, so that it can store/compute everything
class CheckpointManager
{
public:

	CheckpointManager() :
		_mCurrentLap(0),
		_mCurrentCheckpointIndex(0)
	{

	}

	const Checkpoint& GetCurrentCheckpoint() const
	{
		return _mCheckpoints[_mCurrentCheckpointIndex];
	}

	void Update(const Vector2<int>& aCurrentCheckpointTargetPosition, int aAngle, int aDistance)
	{
		// Condition ensuring the init of the checkpoints array when it is empty
		if (_mCheckpoints.empty())
		{
			_mCheckpoints.push_back(Checkpoint(aCurrentCheckpointTargetPosition, aAngle, aDistance));
			return;
		}

		if (_mCheckpoints[_mCurrentCheckpointIndex].Position() == aCurrentCheckpointTargetPosition)
		{
			_mCheckpoints[_mCurrentCheckpointIndex].SetAngle(aAngle);
			_mCheckpoints[_mCurrentCheckpointIndex].SetDistance(aDistance);
			return;
		}

		if (IsFirstCheckpoint(aCurrentCheckpointTargetPosition)) // End of the current Lap
		{
			if (_mCurrentLap == 0)
			{
				AssignBestBoostIndex();
				AssignBestOffset();
			}

			_mCurrentLap++;
			_mCurrentCheckpointIndex = 0;
		}
		else
		{
			_mCurrentCheckpointIndex++;
			if (_mCurrentLap == 0)
				_mCheckpoints.push_back(Checkpoint(aCurrentCheckpointTargetPosition, aAngle, aDistance));
		}
	}

	int ShouldUseBoost()
	{
		return _mCheckpoints[_mCurrentCheckpointIndex].GetBestBoostIndex();
	}

private:

	void AssignBestBoostIndex()
	{
		if (_mCheckpoints.size() < 2)
			std::cerr << "Should atleast have 2 checkpoints";

		float longestDistance = 0.f;
		size_t indexBestBoost = 0;
		for (size_t currentCheckpoint = 0; currentCheckpoint < _mCheckpoints.size(); currentCheckpoint++)
		{
			const size_t nextCheckpoint = (currentCheckpoint + 1) % _mCheckpoints.size();
			float currentDistance = _mCheckpoints[currentCheckpoint].Position().DistanceSqr(_mCheckpoints[nextCheckpoint].Position());

			std::cerr << currentCheckpoint << " - " << nextCheckpoint << " - " << currentDistance << " - " << longestDistance << std::endl;

			if (currentDistance > longestDistance)
			{
				indexBestBoost = nextCheckpoint;
				longestDistance = currentDistance;
			}
		}

		_mCheckpoints[indexBestBoost].SetBestBoostIndex(true);
	}

	void AssignBestOffset()
	{
		if (_mCheckpoints.size() < 2)
			std::cerr << "Should atleast have 2 checkpoints";

		for (size_t currentCheckpoint = 0; currentCheckpoint < _mCheckpoints.size(); currentCheckpoint++)
		{
			const size_t nextCheckpoint = (currentCheckpoint + 1) % _mCheckpoints.size();
			_mCheckpoints[currentCheckpoint].SetNextCheckpointPosition(_mCheckpoints[nextCheckpoint].Position());
		}
	}

	bool IsFirstCheckpoint(const Vector2<int>& aCheckpointPosition)
	{
		return _mCheckpoints.empty() == false && _mCheckpoints[0].Position() == aCheckpointPosition;
	}

private:

	int _mCurrentLap;
	int _mCurrentCheckpointIndex;
	std::vector<Checkpoint> _mCheckpoints;
};
// -------------------------------------------------------------------

class Pod : public Entity
{
private:

	Vector2<int> _mLastFramePosition;

	Vector2<int> _mTarget;

	int _mThrust;
	PodStrategy* _mStrategy;
	Vector2<int> _mSpeed;

	bool _mBoostAvailable;

	int _mShieldCooldown;
	bool _mShieldAvailable;

	bool _mShieldUsed;
	bool _mBoostUsed;

public:

	Pod(Vector2<int> aPosition, int aThrust) :
		Entity(aPosition),
		_mThrust(aThrust),
		_mShieldCooldown(0),
		_mShieldAvailable(true),
		_mBoostAvailable(true),
		_mLastFramePosition(Vector2<int>{-1, 1}),
		_mSpeed(Vector2<int>(0, 0))
	{

	}

	static int MaxThrust;

	bool BoostUsed() const { return _mBoostUsed; }
	bool ShieldUsed() const { return _mShieldUsed; }

	const Vector2<int>& Speed() const { return _mSpeed; }
	const Vector2<int>& Target() const { return _mTarget; }
	int Thrust() const { return _mThrust; }

	void SetTarget(const Vector2<int> aTarget) { _mTarget = aTarget; }

	void RequestBoost()
	{
		if (_mShieldCooldown > 0)
			return;
		if (_mBoostAvailable == false)
			return;

		_mBoostUsed = true;
		_mBoostAvailable = false;
	}

	void RequestShield()
	{
		_mShieldUsed = true;
		_mShieldCooldown = 3;
	}

	Vector2<int> GetLastFramePosition() const
	{
		return _mLastFramePosition;
	}

	void SetThrust(int aValue)
	{
		_mThrust = aValue;
	}

	void SetLastFramePosition(Vector2<int> aValue)
	{
		_mLastFramePosition = aValue;
	}

	std::string Compute(const SimulationEntry& aSimulationEntry);
	void UpdateLastFramePosition(Vector2<int> aPosition);
	void UpdatePosition(Vector2<int> aPosition);
	void UpdateStrategy(PodStrategy& aStrategy);
};


class PodStrategy
{

public:

	PodStrategy()
	{

	}

	virtual bool UseShield(Pod& pod, const SimulationEntry& aSimulationEntry) = 0;
	virtual SimulationResult Compute(Pod& pod, const SimulationEntry& aSimulationEntry) = 0;
};

class StandardPodStrategy : public PodStrategy
{
private:

	CheckpointManager _mCheckpointsManager;
	int _mThresholdHaltPercentage;
	int _mThresholdDistanceBoost;
	int _mThresholdAngle;

public:

	StandardPodStrategy() :
		PodStrategy()
	{
		_mThresholdHaltPercentage = 10;
		_mThresholdDistanceBoost = 300;
		_mThresholdAngle = 2;
	}

	int GetThresholdHaltPercentage() const
	{
		return _mThresholdHaltPercentage;
	}

	int GetThresholdDistanceBoost() const
	{
		return _mThresholdDistanceBoost;
	}

	int GetThresholdAngle() const
	{
		return _mThresholdAngle;
	}

	void SeThresholdHaltPercentage(int aValue)
	{
		_mThresholdHaltPercentage = aValue;
	}

	void SetThresholdDistanceBoost(int aValue)
	{
		_mThresholdDistanceBoost = aValue;
	}

	void SetThresholdAngle(int aValue)
	{
		_mThresholdAngle = aValue;
	}

	bool UseShield(Pod& pod, const SimulationEntry& aSimulationEntry);
	SimulationResult Compute(Pod& pod, const SimulationEntry& aSimulationEntry);
};

// ===================================
// ==== Inline implementations
// ===================================


// ==== Checkpoint ====
float Checkpoint::Radius = 600.0f;

// ==== Pod ====
int Pod::MaxThrust = 100;

inline void Pod::UpdateLastFramePosition(Vector2<int> aPosition)
{
	_mLastFramePosition = aPosition;
}

inline void Pod::UpdatePosition(Vector2<int> aPosition)
{
	_mPosition = aPosition;
	Vector2<int> lastPosition = (_mLastFramePosition.X >= 0) ? (_mLastFramePosition) : (_mPosition);
	_mSpeed = _mPosition - _mLastFramePosition;

	_mShieldUsed = false;
	_mBoostUsed = false;
	if (_mShieldCooldown > 0)
		_mShieldCooldown--;
}

inline void Pod::UpdateStrategy(PodStrategy& aStrategy)
{
	_mStrategy = &aStrategy;
}

inline std::string Pod::Compute(const SimulationEntry& aSimulationEntry)
{
	SimulationResult result = _mStrategy->Compute(*this, aSimulationEntry);
	if (result.Boost)
	{
		std::cerr << "BOOST" << std::endl;
		return std::to_string(result.X) + " " + std::to_string(result.Y) + " " + "BOOST";
	}
	if (result.Shield)
	{
		std::cerr << "SHIELD" << std::endl;
		return std::to_string(result.X) + " " + std::to_string(result.Y) + " " + "SHIELD";
	}

	return std::to_string(result.X) + " " + std::to_string(result.Y) + " " + std::to_string(result.Thrust);
}

// ==== Standard Pod Strategy ====

inline bool StandardPodStrategy::UseShield(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	const Vector2<int> opponentPosition = Vector2<int>(aSimulationEntry.XOpponent, aSimulationEntry.YOpponent);

	const Vector2<int> podNextPosition = pod.Position() + pod.Speed();
	const Vector2<int> opponentNextPosition = aSimulationEntry.PodOpponent.Position() + aSimulationEntry.PodOpponent.Speed();

	const bool collision = podNextPosition.DistanceSqr(opponentNextPosition) < (2 * 240) * (2 * 240);
	std::cerr << "Collision " << collision << std::endl;
	if (collision == false)
		return false;

	const Vector2<float> podDirection = (pod.Target() - pod.Position()).Normalize();
	const Vector2<float> opponentDirectionContact = (opponentPosition - pod.Position()).Normalize();

	std::cerr << "Direction POD : " << podDirection.X << " , " << podDirection.Y << std::endl;
	std::cerr << "Direction Opponent : " << opponentDirectionContact.X << " , " << opponentDirectionContact.Y << std::endl;
	std::cerr << "Contact : " << podDirection.Dot(opponentDirectionContact) << std::endl;


	return podDirection.Dot(opponentDirectionContact) > 0.3f;
}

inline SimulationResult StandardPodStrategy::Compute(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	Vector2<int> currentCheckpointPosition = { aSimulationEntry.XNextCheckpoint, aSimulationEntry.YNextCheckpoint };
	Vector2<int> podPreviousPosition = (pod.GetLastFramePosition().X >= 0) ? (pod.GetLastFramePosition()) : (pod.Position());
	Vector2<int> podTarget = currentCheckpointPosition;

	_mCheckpointsManager.Update(currentCheckpointPosition, aSimulationEntry.AngleNextCheckpoint, aSimulationEntry.DistanceNextCheckpoint);

	Checkpoint _mCurrentCheckpoint = _mCheckpointsManager.GetCurrentCheckpoint();

	std::cerr << "Angle : " << _mCurrentCheckpoint.GetAngle() << std::endl;

	if (UseShield(pod, aSimulationEntry))
	{
		pod.RequestShield();
	}
	else if (_mCurrentCheckpoint.GetAngle() < 2 && _mCurrentCheckpoint.GetDistance() > 650)
	{
		pod.SetThrust(pod.MaxThrust);
		if (_mCheckpointsManager.ShouldUseBoost())
			pod.RequestBoost();
	}
	else
	{
		Vector2<int> position = pod.Position() - pod.GetLastFramePosition();
		podTarget = _mCurrentCheckpoint.Position() - (pod.Speed() * 3.f);

		float distanceFactor = std::clamp(_mCurrentCheckpoint.GetDistance() / (_mCurrentCheckpoint.Radius * 2.f), 0.f, 1.f);
		distanceFactor = std::clamp(distanceFactor * 2.f, 0.f, 1.f);
		float angleFactor = 1.f - std::clamp(_mCurrentCheckpoint.GetAngle() / 90.f, 0.f, 1.f);
		angleFactor = std::clamp(angleFactor * 2.f, 0.f, 1.f);

		std::cerr << "Factors : " << distanceFactor << " - " << angleFactor << std::endl;

		pod.SetThrust(pod.MaxThrust * distanceFactor * angleFactor);
	}

	return SimulationResult(pod.BoostUsed(), pod.ShieldUsed(), podTarget.X, podTarget.Y, pod.Thrust());
}

// ===================================
// ==== Main function
// ===================================
int main()
{

	Pod pod{ Vector2{0, 0}, 50 };
	Pod opponent{ Vector2<int>{0, 0}, 50 };

	StandardPodStrategy strategy;

	pod.UpdateStrategy(strategy);

	while (1) {

		// ============================
		// == Retrieving data from the standard input
		// ============================

		int x;
		int y;
		int next_checkpoint_x; // x position of the next check point
		int next_checkpoint_y; // y position of the next check point
		int next_checkpoint_dist; // distance to the next checkpoint
		int next_checkpoint_angle; // angle between your pod orientation and the direction of the next checkpoint
		std::cin >> x >> y >> next_checkpoint_x >> next_checkpoint_y >> next_checkpoint_dist >> next_checkpoint_angle; std::cin.ignore();
		int opponent_x;
		int opponent_y;
		std::cin >> opponent_x >> opponent_y; std::cin.ignore();

		// ============================
		// == Update and computation
		pod.UpdatePosition({ x, y });
		pod.SetTarget({ next_checkpoint_x, next_checkpoint_y });
		opponent.UpdatePosition({ opponent_x, opponent_y });

		SimulationEntry entry{ x, y, opponent_x, opponent_y, next_checkpoint_x, next_checkpoint_y, abs(next_checkpoint_angle), next_checkpoint_dist, opponent };

		// ============================

		std::cout << pod.Compute(entry) << std::endl;

		pod.UpdateLastFramePosition({ x,y });
		opponent.UpdateLastFramePosition({ opponent_x, opponent_y });

	}
}