#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>


// ===================================
// ==== Constanstes
// ===================================
constexpr float PI = 3.14f;

// ===================================
// ==== Classes forward declaration
// ===================================

class Pod;
class PodStrategy;

// ===================================
// ==== Utility classes
// ===================================

/**
* Vector2 Utility class, used for vector representation as position, speed, etc...
* Provide a nice and encapsulate interface for calculation on vectors
*/
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

	template<typename TO>
	Vector2<TO> ToVector2() const
	{
		return Vector2<TO>(static_cast<TO>(X), static_cast<TO>(Y));
	}

	bool operator==(const Vector2<T>& other) const
	{
		return X == other.X && Y == other.Y;
	}

	Vector2<T> operator+(const Vector2<T>& other) const
	{
		return Vector2<T>(X + other.X, Y + other.Y);
	}
	Vector2<T> operator+=(Vector2<T>& other)
	{
		this = this + other;
	}
	Vector2<T> operator-(const Vector2<T>& other) const
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

	float Magnitude() const
	{
		return sqrt(X*X + Y * Y);
	}

	Vector2<float> Normalize() const
	{
		const float magnitude = Magnitude();
		return Vector2<float>(X / magnitude, Y / magnitude);
	}

	float Dot(const Vector2<T>& other) const
	{
		float result = (X * other.X) + (Y *  other.Y);
		return result;
	}

	float DistanceSqr(const Vector2<T>& other) const
	{
		return (X - other.X)*(X - other.X) + (Y - other.Y)*(Y - other.Y);
	}

	float Distance(const Vector2<T>& other) const
	{
		return sqrt(DistanceSqr(other));
	}
};

/**
* Simulation result is use as computation result for Pod strategies and encapsulate all data needed for the ouput
*/
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

/**
* Entity is the base class for unit that have a position in the word, Pod and checkpoint derives from this class
*/
class Entity
{

protected:

	Vector2<int> _mPosition;

public:

	Entity() :
		_mPosition({ 0, 0 })
	{

	}

	Entity(Vector2<int> aPosition) :
		_mPosition(aPosition)
	{

	}

	// -- Getters
	Vector2<int> Position() const { return _mPosition; }
	// -- Setters
	void SetPosition(Vector2<int> aValue) { _mPosition = aValue; }
};

/**
* Handy way to represent the checkpoint on the race in a class, storing for exemple if this checkpoint
* is the best to boost
* Was mainly use during first phase of the challenge, but informations on checkpoint was not available, so I keep that class
* for the architecture
*/
class Checkpoint : public Entity
{
private:

	bool _mBestBoostIndex;
	Vector2<int> _mNextCheckpointPosition;

public:

	Checkpoint(Vector2<int> aPosition, int aAngle, int aDistance) :
		Entity(aPosition),
		_mBestBoostIndex(false),
		_mNextCheckpointPosition(Vector2<int>{-1, -1})
	{

	}

	Checkpoint(Checkpoint const&) = default;

	Checkpoint& operator=(Checkpoint other)
	{
		_mBestBoostIndex = other._mBestBoostIndex;
		return *this;
	}

	static float Radius;

	// -- Getters
	bool IsBestBoostIndex() const { return _mBestBoostIndex; }

	// -- Setters
	void SetBestBoostIndex(bool aValue) { _mBestBoostIndex = aValue; }
};

/**
* Hold all the checkpoints in the race, providing an handy way to retrieve a checkpoint and its informations
*/
class CheckpointManager
{
private:

	int _mCurrentLap;
	int _mCurrentCheckpointIndex;
	std::vector<Checkpoint> _mCheckpoints;

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

			if (currentDistance > longestDistance)
			{
				indexBestBoost = nextCheckpoint;
				longestDistance = currentDistance;
			}
		}

		_mCheckpoints[indexBestBoost].SetBestBoostIndex(true);
	}

	bool IsFirstCheckpoint(const Vector2<int>& aCheckpointPosition)
	{
		return _mCheckpoints.empty() == false && _mCheckpoints[0].Position() == aCheckpointPosition;
	}

public:

	CheckpointManager() :
		_mCurrentLap(0),
		_mCurrentCheckpointIndex(0)
	{

	}

	int NumberOfCheckpoint() const { return _mCheckpoints.size(); }

	const Checkpoint& GetCurrentCheckpoint() const
	{
		return _mCheckpoints[_mCurrentCheckpointIndex];
	}

	const Checkpoint& operator[](int aIndex) const
	{
		if (aIndex >= _mCheckpoints.size() || aIndex < 0)
		{
			std::cerr << "Index is incorrect" << std::endl;
		}

		return _mCheckpoints[aIndex];
	}

	void BuildFromStandardInput()
	{
		size_t checkpointCount;
		std::cin >> checkpointCount; std::cin.ignore();

		// Reading checkpointCount follow lines for checkpoints
		for (size_t i = 0; i < checkpointCount; i++)
		{
			int checkpointX, checkpointY;
			std::cin >> checkpointX >> checkpointY; std::cin.ignore();
			_mCheckpoints.push_back(Checkpoint({ checkpointX, checkpointY }, 0, 0));
		}

		AssignBestBoostIndex();
	}
};


/**
* Like SimulationResult, provide an encapsulated class to regroup all informations that need to be use for computation of strategies
*/
class SimulationEntry
{
public:

	std::vector<Pod>& myPods;
	std::vector<Pod>& opponentPods;
	const CheckpointManager& checkpointManager;
	int indexRacerOpponent;

	SimulationEntry(std::vector<Pod>& aMyPods, std::vector<Pod>& aOpponentPods, const CheckpointManager& aCheckpointManager, int aIndexRacerOpponent) :
		myPods(aMyPods),
		opponentPods(aOpponentPods),
		checkpointManager(aCheckpointManager),
		indexRacerOpponent(aIndexRacerOpponent)
	{

	}
};


/**
* Class representing a pod, which can be our pod or opponent pod
* The pod is using a PodStrategy based on some algorithm detecting which is racer pod or interceptor pod
*/
class Pod : public Entity
{
private:

	PodStrategy* _mStrategy;

	Vector2<int> _mTarget;
	int _mNextCheckpointIndex;
	int _mThrust;
	int _mCurrentLap;
	Vector2<int> _mSpeed;
	int _mAngle;

	bool _mBoostAvailable;
	bool _mShieldAvailable;
	int _mShieldCooldown;

	bool _mShieldUsed;
	bool _mBoostUsed;

public:


	Pod() :
		Entity({ 0, 0 }),
		_mThrust(Pod::MaxThrust),
		_mShieldCooldown(0),
		_mShieldAvailable(true),
		_mBoostAvailable(true),
		_mSpeed(Vector2<int>(0, 0)),
		_mAngle(0),
		_mNextCheckpointIndex(0),
		_mTarget({ 0,0 }),
		_mCurrentLap(0),
		_mShieldUsed(false),
		_mBoostUsed(false)
	{

	}

	Pod(Vector2<int> aPosition, int aThrust) :
		Entity(aPosition),
		_mThrust(aThrust),
		_mShieldCooldown(0),
		_mShieldAvailable(true),
		_mBoostAvailable(true),
		_mSpeed(Vector2<int>(0, 0)),
		_mAngle(0),
		_mNextCheckpointIndex(0),
		_mTarget({ 0,0 }),
		_mCurrentLap(0),
		_mShieldUsed(false),
		_mBoostUsed(false)
	{

	}

	static int MaxThrust;

	// -- Getters
	bool IsRacer() const;

	bool BoostUsed() const { return _mBoostUsed; }
	bool ShieldUsed() const { return _mShieldUsed; }
	int CurrentLap() const { return _mCurrentLap; }
	int NextCheckpointIndex() const { return _mNextCheckpointIndex; }
	int Angle() const { return _mAngle; }
	const Vector2<int>& Speed() const { return _mSpeed; }
	const Vector2<int>& Target() const { return _mTarget; }
	int Thrust() const { return _mThrust; }

	// -- Setters
	void SetTarget(const Vector2<int> aTarget) { _mTarget = aTarget; }
	void SetThrust(int aValue) { _mThrust = aValue; }

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

	void UpdateFromStandardInput();
	std::string Compute(const SimulationEntry& aSimulationEntry);
	void UpdateStrategy(PodStrategy& aStrategy);
};

/**
* Abstract base class for pod's strategy, providing some useful methods and pure virtual methods about compulation that need to
be define by subclasses
*/
class PodStrategy
{

public:

	PodStrategy()
	{

	}

	// ==
	// This method return the angle that our pod needs to face in order to be align with the position to face
	float ComputeAngle(const Pod& pod, Vector2<int> positionToFace)
	{
		const Vector2<float> direction = (positionToFace - pod.Position()).Normalize();
		float result = acos(direction.X) * 180.f / PI;
		if (direction.Y < 0.f)
			return 360.f - result;
		else
			return result;
	}

	bool Collision(const Pod& podA, const Pod& podB);
	virtual bool BeneficialCollision(const Pod& podA, const Pod& podB) = 0;

	virtual bool IsRacer() const = 0;
	virtual bool ShouldUseShield(Pod& pod, const SimulationEntry& aSimulationEntry) = 0;
	virtual Vector2<int> ComputeTarget(Pod& pod, const SimulationEntry& aSimulationEntry) = 0;
	void ComputeThrust(Pod& pod, const SimulationEntry& aSimulationEntry);
	virtual SimulationResult Compute(Pod& pod, const SimulationEntry& aSimulationEntry) = 0;
};

/**
* Racer strategy aiming to make the pod with this strategy win the race
*/
class RacerPodStrategy : public PodStrategy
{
private:

	int _mThresholdHaltPercentage;
	int _mThresholdDistanceBoost;
	int _mThresholdAngle;

public:

	RacerPodStrategy() :
		PodStrategy()
	{
		_mThresholdHaltPercentage = 10;
		_mThresholdDistanceBoost = 300;
		_mThresholdAngle = 2;
	}

	bool IsRacer() const { return true; }

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

	Vector2<int> ComputeTarget(Pod& pod, const SimulationEntry& aSimulationEntry);
	SimulationResult Compute(Pod& pod, const SimulationEntry& aSimulationEntry);

	bool BeneficialCollision(const Pod& podA, const Pod& podB);
	bool ShouldUseShield(Pod& pod, const SimulationEntry& aSimulationEntry);

};

/**
* Interceptor strategy aiming to interrupt opponent pod in its path to victory
*/
class InterceptorPodStrategy : public PodStrategy
{
public:

	InterceptorPodStrategy() :
		PodStrategy()
	{

	}

	bool IsRacer() const { return false; }

	Vector2<int> ComputeTarget(Pod& pod, const SimulationEntry& aSimulationEntry);
	bool BeneficialCollision(const Pod& podA, const Pod& podB);
	bool ShouldUseShield(Pod& pod, const SimulationEntry& aSimulationEntry);
	SimulationResult Compute(Pod& pod, const SimulationEntry& aSimulationEntry);
};

// ===================================
// ==== Inline implementations
// ===================================


// ==== Checkpoint ====
float Checkpoint::Radius = 600.0f;

// ==== Pod ====
int Pod::MaxThrust = 100;

inline bool Pod::IsRacer() const
{
	return _mStrategy->IsRacer();
}

inline void Pod::UpdateFromStandardInput()
{
	int x, y, vx, vy, angle, nextCheckpointIndex;
	std::cin >> x >> y >> vx >> vy >> angle >> nextCheckpointIndex; std::cin.ignore();

	_mPosition = { x,y };
	_mSpeed = { vx, vy };
	_mAngle = angle;
	if (nextCheckpointIndex != _mNextCheckpointIndex)
	{
		_mNextCheckpointIndex = nextCheckpointIndex;
		if (nextCheckpointIndex == 0)
		{
			_mCurrentLap++;
		}
	}

	_mShieldUsed = false;
	_mBoostUsed = false;
	if (_mShieldCooldown > 0)
	{
		_mShieldCooldown--;
	}
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


// ==== Pod Strategy ====
inline bool PodStrategy::Collision(const Pod& podA, const Pod& podB)
{
	const Vector2<int> podANextPosition = podA.Position() + podA.Speed();
	const Vector2<int> podBNextPosition = podB.Position() + podB.Speed();
	bool collision = (podANextPosition.DistanceSqr(podBNextPosition) < ((2 * 420) * (2 * 420)));

	return collision;
}

inline void PodStrategy::ComputeThrust(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	Checkpoint podTargetCheckpoint = aSimulationEntry.checkpointManager[pod.NextCheckpointIndex()];

	if (ShouldUseShield(pod, aSimulationEntry))
	{
		pod.RequestShield();
	}

	float actualAngle = ComputeAngle(pod, pod.Target());
	float angleDifference = abs((int)(actualAngle - pod.Angle()));

	if (angleDifference < 2 && pod.Target().Distance(pod.Position()) > 650)
	{
		pod.SetThrust(pod.MaxThrust);

		if (podTargetCheckpoint.IsBestBoostIndex())
			pod.RequestBoost();
	}
	else
	{
		const Vector2<int> targetPosition = pod.Target();
		// Try to minimize drifting
		pod.SetTarget(targetPosition - (pod.Speed() * 3.f));

		float distanceToTarget = targetPosition.Distance(pod.Position());
		float distanceSlowdownFactor = std::clamp(distanceToTarget / (2.f * Checkpoint::Radius), 0.f, 1.f);

		float angleToTarget = ComputeAngle(pod, pod.Target());
		float angle = angleToTarget - pod.Angle();
		float angleSlowdownFactor = 1.f - std::clamp(abs((int)angle) / 90.f, 0.f, 1.f);

		pod.SetThrust(pod.MaxThrust * distanceSlowdownFactor * angleSlowdownFactor);
	}
}

// ==== Standard Pod Strategy ====

inline bool RacerPodStrategy::BeneficialCollision(const Pod& podA, const Pod& podB)
{
	// Dot product of two normalized vectors gives a value between -1 and 1
	// 0 = Vectors are perpendicular
	// 1 = Vectors are parallel (facing the same direction)
	// -1 = Vectors are anti-parallel (facing the opposite direction)
	// 
	// 0.5 seems a reasonable value, meaning the pod will still be more or less pushed into the good direction
	const Vector2<float> podADirection = (podA.Target() - podA.Position()).Normalize();
	const Vector2<float> podBDirectionSmashingA = (podA.Position() - podB.Position()).Normalize();
	std::cerr << podA.Position().X << " - " << podB.Position().X << std::endl;
	std::cerr << "Direction X " << (podA.Position() - podB.Position()).X << std::endl;
	float dot = podADirection.Dot(podBDirectionSmashingA);
	return dot > 0.5f;
}

inline bool RacerPodStrategy::ShouldUseShield(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	Pod& otherPod = (aSimulationEntry.myPods[0].IsRacer()) ? (aSimulationEntry.myPods[1]) : (aSimulationEntry.myPods[0]);
	Pod& opponentPod1 = aSimulationEntry.opponentPods[0];
	Pod& opponentPod2 = aSimulationEntry.opponentPods[1];

	if (Collision(pod, otherPod))
	{
		if (BeneficialCollision(pod, otherPod))
			return false;

		//return true;
		return true;
	}

	if (Collision(pod, opponentPod1))
	{
		if (BeneficialCollision(pod, opponentPod1))
			return false;

		return true;
	}

	if (Collision(pod, opponentPod2))
	{
		if (BeneficialCollision(pod, opponentPod2))
			return false;

		return true;
	}

	return false;
}

inline Vector2<int> RacerPodStrategy::ComputeTarget(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	Checkpoint nextNextCheckpoint = aSimulationEntry.checkpointManager[(pod.NextCheckpointIndex() + 1) % aSimulationEntry.checkpointManager.NumberOfCheckpoint()];

	Vector2<int> nextPosition = pod.Position() + pod.Speed();
	std::cerr << nextPosition.Distance(pod.Target()) << std::endl;
	if (nextPosition.Distance(pod.Target()) < 1400)
	{
		return nextNextCheckpoint.Position();
	}
	else
		return aSimulationEntry.checkpointManager[pod.NextCheckpointIndex()].Position();
}

inline SimulationResult RacerPodStrategy::Compute(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	pod.SetTarget(ComputeTarget(pod, aSimulationEntry));
	ComputeThrust(pod, aSimulationEntry);
	return SimulationResult(pod.BoostUsed(), pod.ShieldUsed(), pod.Target().X, pod.Target().Y, pod.Thrust());

}

// == Interceptor Pod Strategy

inline Vector2<int> InterceptorPodStrategy::ComputeTarget(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	Vector2<int> target = { 0, 0 };

	Pod& opponent = aSimulationEntry.opponentPods[aSimulationEntry.indexRacerOpponent];
	Vector2<int> opponentTarget = aSimulationEntry.checkpointManager[opponent.NextCheckpointIndex()].Position();
	Vector2<int> opponentDirection = (opponentTarget - pod.Position());
	Vector2<int> direction = (opponent.Position() - pod.Position());

	bool canReachOpponentRacer = [&]() -> bool
	{
		// If dot product return a value higher than 0, it's mean they face the same direction
		// And pushing the opponentRacer would not be efficient
		if (direction.Dot(opponentDirection) > 0)
			return false;

		// If opponent pod is closer to his target than our interceptor, we can't reach it before him
		if (pod.Position().DistanceSqr(opponentTarget) > opponent.Position().DistanceSqr(opponentTarget))
			return false;

		return true;
	}();

	if (canReachOpponentRacer)
	{
		return opponent.Position() + opponent.Speed()* 3.f;
	}
	// Because our interceptor is not able to reach the opponent pod at this stage, we'll check where we can wait it
	// To do it, we'll check checkpoint by checkpoint, adding distance separating each other in a quite rough computation
	// And stop when we see that our pod can reach it before opponent pod
	float opponentDistance = 0;
	Vector2<int> opponentCheckpointPositionToReach = opponent.Position();
	for (int i = 0; i < aSimulationEntry.checkpointManager.NumberOfCheckpoint(); i++)
	{
		int nextCheckpointIndex = (opponent.NextCheckpointIndex() + 1) % (aSimulationEntry.checkpointManager.NumberOfCheckpoint());
		Vector2<int> nextCheckpointPosition = aSimulationEntry.checkpointManager[nextCheckpointIndex].Position();

		opponentDistance += opponentCheckpointPositionToReach.Distance(nextCheckpointPosition);
		float podDistance = pod.Position().Distance(nextCheckpointPosition);

		opponentCheckpointPositionToReach = nextCheckpointPosition;
		if (podDistance < opponentDistance)
			break;
	}

	return opponentCheckpointPositionToReach;
}

inline bool InterceptorPodStrategy::BeneficialCollision(const Pod& podA, const Pod& podB)
{
	// Dot product of two normalized vectors gives a value between -1 and 1
	// 0 = Vectors are perpendicular
	// 1 = Vectors are parallel (facing the same direction)
	// -1 = Vectors are anti-parallel (facing the opposite direction)
	// 
	// 0.5 seems a reasonable value, meaning the pod will push his target into a quite undesirable position
	const Vector2<float> podADirection = (podA.Target() - podA.Position()).Normalize();
	const Vector2<float> podBDirection = (podB.Target() - podB.Position()).Normalize();
	return podADirection.Dot(podBDirection) < 0.5f;
}

inline bool InterceptorPodStrategy::ShouldUseShield(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	Pod& otherPod = (aSimulationEntry.myPods[0].IsRacer()) ? (aSimulationEntry.myPods[0]) : (aSimulationEntry.myPods[1]);
	Pod& opponentPod1 = aSimulationEntry.opponentPods[0];
	Pod& opponentPod2 = aSimulationEntry.opponentPods[1];

	if (Collision(pod, otherPod))
	{
		if (BeneficialCollision(pod, otherPod))
			return false;

		return true;
	}

	if (Collision(pod, opponentPod1))
	{
		if (BeneficialCollision(pod, opponentPod1))
			return true;

		return false;
	}

	if (Collision(pod, opponentPod2))
	{
		if (BeneficialCollision(pod, opponentPod2))
			return true;

		return false;
	}

	return false;
}

inline SimulationResult InterceptorPodStrategy::Compute(Pod& pod, const SimulationEntry& aSimulationEntry)
{
	pod.SetTarget(ComputeTarget(pod, aSimulationEntry));
	ComputeThrust(pod, aSimulationEntry);
	return SimulationResult(pod.BoostUsed(), pod.ShieldUsed(), pod.Target().X, pod.Target().Y, pod.Thrust());
}

// ===================================
// ==== Main function
// ===================================

// Return positive value if first pod is ahead of second pod, negative otherwise
int IndexPodAhead(const Pod& firstPod, const Pod& secondPod, const CheckpointManager& aCheckpointManager)
{
	if (firstPod.CurrentLap() != secondPod.CurrentLap())
	{
		return firstPod.CurrentLap() > secondPod.CurrentLap();
	}

	if (firstPod.NextCheckpointIndex() != secondPod.NextCheckpointIndex())
	{
		return firstPod.NextCheckpointIndex() > secondPod.NextCheckpointIndex();
	}

	size_t indexCheckpointFirstPod = firstPod.NextCheckpointIndex();
	const Vector2<int>& positionNextCheckpoint = aCheckpointManager[indexCheckpointFirstPod].Position();
	return firstPod.Position().DistanceSqr(positionNextCheckpoint) < secondPod.Position().DistanceSqr(positionNextCheckpoint);
}

int main()
{
	CheckpointManager checkpointsManager;
	std::vector<Pod> myPods(2);
	std::vector<Pod> opponentPods(2);

	RacerPodStrategy racerStrategy;
	InterceptorPodStrategy interceptorStrategy;

	// Variables holding input
	size_t laps;

	// Lap count & initializing checkpoint manager from input
	std::cin >> laps; std::cin.ignore();
	checkpointsManager.BuildFromStandardInput();

	while (1) {

		// -- Our pods and opponent update from standard entry
		for (Pod& pod : myPods)
		{
			pod.UpdateFromStandardInput();
		}

		for (Pod& opponentPod : opponentPods)
		{
			opponentPod.UpdateFromStandardInput();
		}

		int indexRacingPod = (IndexPodAhead(myPods[0], myPods[1], checkpointsManager)) ? (0) : (1);
		int indexOpponentAhead = (IndexPodAhead(opponentPods[0], opponentPods[1], checkpointsManager)) ? (0) : (1);
		Pod& racingPod = myPods[indexRacingPod];
		Pod& interceptorPod = myPods[1 - indexRacingPod];

		racingPod.UpdateStrategy(racerStrategy);
		interceptorPod.UpdateStrategy(interceptorStrategy);

		// -- Building our simulation entry for this turn

		SimulationEntry entry{ myPods, opponentPods, checkpointsManager, indexOpponentAhead };

		// ============================ 
		std::cout << racingPod.Compute(entry) << std::endl;
		std::cout << interceptorPod.Compute(entry) << std::endl;

	}
}