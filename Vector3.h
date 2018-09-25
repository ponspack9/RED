template <typename T> class Vector3 {

public:

	T x;
	T y;
	T z;

public:

	// CONSTRUCTORS

	Vector3() {}
	
	Vector3(const T &val) :
		x(val),
		y(val),
		z(val)
	{}

	Vector3(const Vector3 & v) :
		x(v.x),
		y(v.y),
		z(v.z)
	{}

	Vector3(const T & x, const T & y, const T & z) :
		x(x),
		y(y),
		z(z)
	{}


	// OPERATORS MODIFIERS

	const Vector3<T>& operator += (const T & val)
	{
		x += val;
		y += val;
		z += val;

		return (*this);
	}

	const Vector3<T>& operator -= (const T & val)
	{
		x -= val;
		y -= val;
		z -= val;

		return (*this);
	}

	const Vector3<T>& operator =  (const T & val)
	{
		x = val;
		y = val;
		z = val;

		return (*this);
	}


	const Vector3<T>& operator += (const Vector3 & v)
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return (*this);
	}

	const Vector3<T>& operator -= (const Vector3 & v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return (*this);
	}

	const Vector3<T>& operator =  (const Vector3 & v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return (*this);
	}


	// OPERATORS

	Vector3<T> operator + (const T & val) const
	{
		return Vector3<T>(x + val, y + val, z + val);
	}

	Vector3<T> operator - (const T & val) const
	{
		return Vector3<T>(x - val, y - val, z - val);
	}

	bool operator == (const T & val) const
	{
		return x == val && y == val && z == val;
	}

	bool operator != (const T & val) const
	{
		return x != val || y != val || z != val;
	}


	Vector3<T> operator + (const Vector3 & v) const
	{
		return Vector3<T>(x + v.x, y + v.y, z + v.z);
	}

	Vector3<T> operator - (const Vector3 & v) const
	{
		return Vector3<T>(x - v.x, y - v.y, z - v.z);
	}

	bool operator == (const Vector3 & v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator != (const Vector3 & v) const
	{
		return x != v.x || y != v.y || z != v.y;
	}


	// MODIFIER FUNCTIONS
	
	const Vector3<T>& Normalize()
	{
		T mod = sqrt((x*x) + (y*y) + (z*z));
		if (mod != 0) {
			x = x / mod;
			y = y / mod;
			z = z / mod;
		}
		// Not sure about this
		/*else {
			x = y = z = 0;
		}*/
		return (*this);
	}

	const Vector3<T>& Zero()
	{
		x = 0;
		y = 0;
		z = 0;

		return (*this);
	}


	// QUERY FUNCTIONS
	
	bool IsZero() const
	{
		return x == 0 && y == 0 && z == 0;
	}

	void Print() const {
		cout << x << " " << y << " " << z << endl;
	}

	T Distance_to(const Vector3 & v) const
	{
		T x_ = v.x - x;
		T y_ = v.y - y;
		T z_ = v.z - z;

		return sqrt(x_*x_ + y_* y_ + z_* z_);
	}

	T Magnitude() const {
		return sqrt(x*x + y*y + z*z);
	}

};