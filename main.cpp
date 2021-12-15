#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


// ===================================
// ==== Classes forward declaration
// ===================================

class Pod;
class PodStrategy;

// ===================================
// ==== Utility classes
// ===================================

template<typename T, typename C>
class Property
{
	using SetterType = void (C::*)(T);
	using GetterType = T(C::*)() const;

private:

	C* const _mObject;
	SetterType const _mSetter;
	GetterType const _mGetter;

public:

	Property(C* aObject, SetterType aSetter, GetterType aGetter) :
		_mObject(aObject),
		_mSetter(aSetter),
		_mGetter(aGetter)
	{

	}

	operator T() const
	{
		return (_mObject->*_mGetter)();
	}

	C& operator = (T aValue)
	{
		(_mObject->*_mSetter)(aValue);
		return *_mObject;
	}
};

template <typename T>
class Vector2
{
	static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value || std::is_same<T, int>::value, "Type is not valid");

private:

	T _mX;
	T _mY;

	T GetX() const
	{
		return _mX;
	}

	T GetY() const
	{
		return _mY;
	}

	void SetX(int aValue)
	{
		_mX = aValue;
	}

	void SetY(int aValue)
	{
		_mY = aValue;
	}

public:

	Property<T, Vector2> X{ this, &Vector2::SetX, &Vector2::GetX };
	Property<T, Vector2> Y{ this, &Vector2::SetY, &Vector2::GetY };

	Vector2(T aX, T aY) :
		_mX(aX),
		_mY(aY)
	{

	}

	Vector2<T>(Vector2<T> const&) = default;

	Vector2<T>& operator=(Vector2<T> other)
	{
		_mX = other.X;
		_mY = other.Y;
		return *this;
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

	Vector2<int> GetPosition() const
	{
		return _mPosition;
	}
};


class Checkpoint : public Entity
{
private:

	int _mAngle;
	int _mDistance;


	int GetAngle() const
	{
		return _mAngle;
	}

	int GetDistance() const
	{
		return _mDistance;
	}

	void SetAngle(int aValue)
	{
		_mAngle = aValue;
	}

	void SetDistance(int aValue)
	{
		_mDistance = aValue;
	}

public:

	Checkpoint(Vector2<int> aPosition, int aAngle, int aDistance) :
		Entity(aPosition),
		_mAngle(aAngle),
		_mDistance(aDistance)
	{

	}

	Checkpoint(Checkpoint const&) = default;

	Checkpoint& operator=(Checkpoint other)
	{
		_mAngle = other._mAngle;
		_mDistance = other._mDistance;
		return *this;
	}

	Property<int, Checkpoint> Angle{ this, &Checkpoint::SetAngle, &Checkpoint::GetAngle };
	Property<int, Checkpoint> Distance{ this, &Checkpoint::SetDistance, &Checkpoint::GetDistance };
};

class Pod : public Entity
{
private:

	bool _mBoostAvailable;
	int _mThrust;
	const PodStrategy* _mStrategy;

	bool GetBoostAvailable() const
	{
		return _mBoostAvailable;
	}

	int GetThrust() const
	{
		return _mThrust;
	}

	void SetBoostAvailable(bool aValue)
	{
		_mBoostAvailable = aValue;
	}

	void SetThrust(int aValue)
	{
		_mThrust = aValue;
	}

public:

	Pod(Vector2<int> aPosition, int aThrust) :
		Entity(aPosition),
		_mThrust(aThrust)
	{
		_mBoostAvailable = true;
	}

	Property<bool, Pod> BoostAvailable{ this, &Pod::SetBoostAvailable, &Pod::GetBoostAvailable };
	Property<int, Pod> Thrust{ this, &Pod::SetThrust, &Pod::GetThrust };

	void UpdatePosition(Vector2<int> aPosition);
	void UpdateStrategy(const PodStrategy& aStrategy);

	Vector2<int> Move(const Checkpoint& aNextCheckpoint);
	int Boost(const Checkpoint& aNextCheckpoint);

};


class PodStrategy
{

public:

	PodStrategy()
	{

	}

	virtual Vector2<int> ComputeNextPosition(Pod& pod, const Checkpoint& aNextCheckpoint) const = 0;
	virtual int ComputeNextBoost(Pod& pod, const Checkpoint& aNextCheckpoint) const = 0;
};

class StandardPodStrategy : public PodStrategy
{
private:

	int _mThresholdHaltPercentage;
	int _mThresholdDistanceBoost;

	int GetThresholdHaltPercentage() const
	{
		return _mThresholdHaltPercentage;
	}

	int GetThresholdDistanceBoost() const
	{
		return _mThresholdDistanceBoost;
	}

	void SeThresholdHaltPercentage(int aValue)
	{
		_mThresholdHaltPercentage = aValue;
	}

	void SetThresholdDistanceBoost(int aValue)
	{
		_mThresholdDistanceBoost = aValue;
	}

public:

	StandardPodStrategy() :
		PodStrategy()
	{
		_mThresholdHaltPercentage = 10;
		_mThresholdDistanceBoost = 100;
	}

	Property<int, StandardPodStrategy> ThresholdHaltPercentage{ this, &StandardPodStrategy::SeThresholdHaltPercentage, &StandardPodStrategy::GetThresholdHaltPercentage };
	Property<int, StandardPodStrategy> ThresholdDistanceBoost{ this, &StandardPodStrategy::SetThresholdDistanceBoost, &StandardPodStrategy::GetThresholdDistanceBoost };

	Vector2<int> ComputeNextPosition(Pod& pod, const Checkpoint& aNextCheckpoint) const;
	int ComputeNextBoost(Pod& pod, const Checkpoint& aNextCheckpoint) const;
};

// ===================================
// ==== Inline implementations
// ===================================

// ==== Pod ====
inline void Pod::UpdatePosition(Vector2<int> aPosition)
{
	_mPosition = aPosition;
}

inline void Pod::UpdateStrategy(const PodStrategy& aStrategy)
{
	_mStrategy = &aStrategy;
}

inline Vector2<int> Pod::Move(const Checkpoint &aNextCheckpoint)
{
	return _mStrategy->ComputeNextPosition(*this, aNextCheckpoint);
}

inline int Pod::Boost(const Checkpoint& aNextCheckpoint)
{
	return _mStrategy->ComputeNextBoost(*this, aNextCheckpoint);
}

// ==== Standard Pod Strategy ====
inline Vector2<int> StandardPodStrategy::ComputeNextPosition(Pod& pod, const Checkpoint& aNextCheckpoint) const
{
	return { aNextCheckpoint.GetPosition().X, aNextCheckpoint.GetPosition().Y };
}

inline int StandardPodStrategy::ComputeNextBoost(Pod& pod, const Checkpoint& aNextCheckpoint) const
{
	if (aNextCheckpoint.Distance >= _mThresholdDistanceBoost && pod.BoostAvailable)
	{
		pod.BoostAvailable = false;
		return -1;
	}

	if (aNextCheckpoint.Angle > 90 || aNextCheckpoint.Angle < -90)
		return 0;

	return 100;
}

// ===================================
// ==== Main function
// ===================================
int main()
{

	Pod pod{ Vector2{0, 0}, 50 };
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
		// == Building variables and populating classes
		// ============================

		Checkpoint nextCheckpoint{ Vector2{next_checkpoint_x, next_checkpoint_y}, next_checkpoint_angle, next_checkpoint_dist };

		// ============================
		// == Update and computation
		pod.UpdatePosition({ x, y });

		Vector2<int> computedPosition = pod.Move(nextCheckpoint);
		int computedThrust = pod.Boost(nextCheckpoint);
		// ============================

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;

		if (computedThrust == -1)
		{
			std::cout << computedPosition.X << " " << computedPosition.Y << " " << "BOOST" << std::endl;
		}
		else
		{
			std::cout << computedPosition.X << " " << computedPosition.Y << " " << computedThrust << std::endl;
		}

	}
}