/** \addtogroup typelibrary
*  @{
*/

/*!
\file any.h
*/
#ifndef _ANY_H_
#define _ANY_H_
#include <typeinfo>
#include <memory>

namespace ipengine {
	/*!
	\brief Implements the type erasure idiom.
	*/
	class any
	{
	private:
		//CONCEPT
		struct any_concept
		{
			using ptr = std::unique_ptr<any_concept>;
			virtual ~any_concept() {}
			virtual const std::type_info& type() const = 0;
			virtual any_concept* clone() const = 0;
			virtual size_t size() const = 0;
		};

		//MODEL
		template <typename T>
		struct any_model : public any_concept
		{
			any_model(T _val) :
				value(std::move(_val))
			{
			}

			virtual const std::type_info & type() const override
			{
				return typeid(T);
			}

			virtual any_concept * clone() const override
			{
				return new any_model(value);
			}

			virtual size_t size() const override
			{
				return sizeof(T);
			}

			T& operator*()
			{
				return value;
			}

			const T& operator*() const
			{
				return value;
			}

		private:
			T value;
		};

	public:
		//PUBLIC INTERFACE
		//! Default constructor
		any() :
			m_value(std::make_unique<any_model<int>>(0))
		{
		}

		/*!
		\brief Initializes the any object with some arbitrary data.
		\tparam T		Type of the data.
		\param[in] val	Data.
		*/
		template <typename T>
		any(T val) :
			m_value(std::make_unique<any_model<T>>(std::forward<T>(val)))
		{
		}

		/*!
		\brief Assigns new data to the any object
		\tparam T		Type of the data.
		\param[in] val	Data.
		*/
		template <typename T>
		any& operator=(T val)
		{
			m_value = std::make_unique<any_model<T>>(std::forward<T>(val));
			return *this;
		}

		/*!
		\brief Copy constructor.
		*/
		any(const any& other) :
			m_value(other.m_value->clone())
		{
		}

		/*!
		\brief Move constructor.
		*/
		any(any&& other) :
			m_value(std::move(other.m_value))
		{
		}

		/*!
		\brief Copy assignment.
		*/
		any& operator=(const any& other)
		{
			if (this == &other)
				return *this;
			any tmp(other);
			swap(tmp);
			return *this;
		}

		/*!
		\brief Move assignment.
		*/
		any& operator=(any&& other) noexcept
		{
			if (this == &other)
				return *this;
			m_value = std::move(other.m_value);
			return *this;
		}

		/*!
		\brief Swaps content of two any's.
		*/
		void swap(any& other) noexcept
		{
			m_value.swap(other.m_value);
		}

		/*!
		\brief Returns the size of the held object.
		*/
		size_t size() const
		{
			return m_value->size();
		}

		/*!
		\brief returns std::type_info for the held object.
		*/
		const std::type_info& type() const
		{
			return m_value->type();
		}

		/*!
		\brief Casts the held content to T&.
		\tparam		Desired type.
		\returns	Returns a reference to T.
		\throws		Throws std::bad_cast if the content cannot be converted to T.
		*/
		template <typename T>
		T& cast()
		{
			return *dynamic_cast<any_model<T>&>(*m_value);
		}

		/*!
		\brief Casts the held content to const T&.
		\tparam		Desired type.
		\returns	Returns a const reference to T.
		\throws		Throws std::bad_cast if the content cannot be converted to T.
		*/
		template <typename T>
		const T& cast() const
		{
			return *dynamic_cast<any_model<T>&>(*m_value);
		}

		/*!
		\brief Implicit conversion to T&.
		*/
		template <typename T>
		operator T&()
		{
			return cast<T>();
		}

		/*!
		\brief Implicit conversion to const T&.
		*/
		template <typename T>
		operator const T&() const
		{
			return cast<T>();
		}

		/*!
		\brief Destroys the held object.
		*/
		void clear()
		{
			m_value = std::make_unique<any_model<int>>(0);
		}


	private:
		any_concept::ptr m_value;
	};


	//sizeof(soo_any) is 32 bytes. SOO will work with up to 16 bytes: 8 bytes are reserved for the vtable pointer. another 8 bytes are needed for the "small" flag.
#define ANY_SOB_SIZE 24

	/*!
	\brief Implements the type erasure idiom with small object optimization.

	Small object optimization prevents heap allocations for object sizes up to 16 bytes.
	*/
	class soo_any
	{
	private:
		//CONCEPT
		struct soo_any_concept
		{
			using ptr = std::unique_ptr<soo_any_concept>;
			virtual ~soo_any_concept() {}
			virtual const std::type_info& type() const = 0;
			virtual soo_any_concept* clone() const = 0;
			virtual soo_any_concept* move() = 0;
			virtual soo_any_concept* placement_clone(void* ptr) const = 0;
			virtual soo_any_concept* placement_move(void* ptr) = 0;
			virtual size_t size() const = 0;
			virtual size_t internal_size() const = 0;
		};

		//MODEL
		template <typename T>
		struct soo_any_model : public soo_any_concept
		{
			soo_any_model(T _val) :
				value(std::move(_val))
			{
			}

			virtual const std::type_info & type() const override
			{
				return typeid(T);
			}

			virtual soo_any_concept * clone() const override
			{
				return new soo_any_model(value);
			}

			virtual soo_any_concept * move() override
			{
				return new soo_any_model(std::move(value));
			}

			virtual soo_any_concept * placement_clone(void* ptr) const override
			{
				return new(ptr)soo_any_model(value);
			}

			virtual soo_any_concept * placement_move(void* ptr) override
			{
				return new(ptr)soo_any_model(std::move(value));
			}

			virtual size_t size() const override
			{
				return sizeof(T);
			}

			virtual size_t internal_size() const override
			{
				return sizeof(soo_any_model<T>);
			}

			T& operator*()
			{
				return value;
			}

			const T& operator*() const
			{
				return value;
			}

		private:
			T value;
		};

	public:
		//PUBLIC INTERFACE
		//! Default constructor
		soo_any()
		{
			new(reinterpret_cast<void*>(&m_value))soo_any_model<int>(0);
			small = true;
			//m_value = nullptr;
		}

		~soo_any()
		{
			destruct_old();
		}

		void destruct_old()
		{
			if (small)
			{
				reinterpret_cast<soo_any_concept*>(&m_value)->~soo_any_concept();
				new(reinterpret_cast<void*>(&m_value))soo_any_model<int>(0);
				small = true;
			}
			else
			{
				delete m_value;
				new(reinterpret_cast<void*>(&m_value))soo_any_model<int>(0);
				small = true;
			}
		}

		/*!
		\brief Initializes the any object with some arbitrary data.
		\tparam T		Type of the data.
		\param[in] val	Data.
		*/
		template <typename T>
		soo_any(T val)
		{
			if (sizeof(soo_any_model<T>) > ANY_SOB_SIZE)
			{
				m_value = new soo_any_model<T>(std::forward<T>(val));
				small = false;
			}
			else
			{
				new(reinterpret_cast<void*>(&m_value))soo_any_model<T>(std::forward<T>(val));
				small = true;
			}
		}

		/*!
		\brief Assigns new data to the any object
		\tparam T		Type of the data.
		\param[in] val	Data.
		*/
		template <typename T>
		soo_any& operator=(T val)
		{
			destruct_old();
			if (sizeof(soo_any_model<T>) > ANY_SOB_SIZE)
			{
				m_value = new soo_any_model<T>(std::forward<T>(val));
				small = false;
			}
			else
			{
				new(reinterpret_cast<void*>(&m_value))soo_any_model<T>(std::forward<T>(val));
				small = true;
			}

			return *this;
			/*m_value = std::make_unique<soo_any_model<T>>(std::forward<T>(val));
			return *this;*/
		}

		/*!
		\brief Copy constructor.
		*/
		soo_any(const soo_any& other)
		{
			if (other.small)
			{
				reinterpret_cast<const soo_any_concept*>(&other.m_value)->placement_clone(reinterpret_cast<void*>(&m_value));
				small = true;
			}
			else
			{
				m_value = other.m_value->clone();
				small = false;
			}
		}

		/*!
		\brief Move constructor.
		*/
		soo_any(soo_any&& other)
		{
			if (other.small)
			{
				reinterpret_cast<soo_any_concept*>(&other.m_value)->placement_move(reinterpret_cast<void*>(&m_value));
				new(reinterpret_cast<void*>(&other.m_value))soo_any_model<int>(0);
				other.small = true;
				small = true;
			}
			else
			{
				m_value = other.m_value;
				new(reinterpret_cast<void*>(&other.m_value))soo_any_model<int>(0);
				other.small = true;
				small = false;
			}
		}

		/*!
		\brief Copy assignment.
		*/
		soo_any& operator=(const soo_any& other)
		{
			if (this == &other)
				return *this;
			destruct_old();
			if (other.small)
			{
				reinterpret_cast<const soo_any_concept*>(&other.m_value)->placement_clone(reinterpret_cast<void*>(&m_value));
				small = true;
			}
			else
			{
				m_value = other.m_value->clone();
				small = false;
			}
			return *this;
		}

		/*!
		\brief Move assignment.
		*/
		soo_any& operator=(soo_any&& other)
		{
			if (this == &other)
				return *this;
			destruct_old();
			if (other.small)
			{
				reinterpret_cast<soo_any_concept*>(&other.m_value)->placement_move(reinterpret_cast<void*>(&m_value));
				new(reinterpret_cast<void*>(&other.m_value))soo_any_model<int>(0);
				other.small = true;
				small = true;
			}
			else
			{
				m_value = other.m_value;
				new(reinterpret_cast<void*>(&other.m_value))soo_any_model<int>(0);
				other.small = true;
				small = false;
			}
			return *this;
		}

		/*!
		\brief Swaps content of two soo_any's.
		*/
		void swap(soo_any& other)
		{
			if (other.small)
			{
				if (small)
				{
					unsigned char tmp[ANY_SOB_SIZE];
					reinterpret_cast<soo_any_concept*>(&other.m_value)->placement_move(static_cast<void*>(tmp));
					reinterpret_cast<soo_any_concept*>(&m_value)->placement_move(static_cast<void*>(&other.m_value));
					reinterpret_cast<soo_any_concept*>(&tmp)->placement_move(static_cast<void*>(&m_value));
					reinterpret_cast<soo_any_concept*>(&tmp)->~soo_any_concept();
				}
				else
				{
					unsigned char tmp[ANY_SOB_SIZE];
					reinterpret_cast<soo_any_concept*>(&other.m_value)->placement_move(static_cast<void*>(tmp));
					other.small = false;
					other.m_value = m_value;
					reinterpret_cast<soo_any_concept*>(&tmp)->placement_move(static_cast<void*>(&m_value));
					reinterpret_cast<soo_any_concept*>(&tmp)->~soo_any_concept();
					small = true;
				}
			}
			else
			{
				if (small)
				{
					soo_any_concept* tmp = other.m_value;
					reinterpret_cast<soo_any_concept*>(&m_value)->placement_move(static_cast<void*>(&other.m_value));
					other.small = true;
					reinterpret_cast<soo_any_concept*>(&m_value)->~soo_any_concept();
					m_value = tmp;
					small = false;
				}
				else
				{
					soo_any_concept* tmp = other.m_value;
					other.m_value = m_value;
					m_value = tmp;
				}
			}
		}

		/*!
		\brief Returns the size of the held object.
		*/
		size_t size() const
		{
			if (small)
			{
				return reinterpret_cast<const soo_any_concept*>(&m_value)->size();
			}
			else
			{
				return m_value->size();
			}

		}

		/*!
		\brief returns std::type_info for the held object.
		*/
		const std::type_info& type() const
		{
			if (small)
			{
				return reinterpret_cast<const soo_any_concept*>(&m_value)->type();
			}
			else
			{
				return m_value->type();
			}
		}

		/*!
		\brief Casts the held content to T&.
		\tparam		Desired type.
		\returns	Returns a reference to T.
		\throws		Throws std::bad_cast if the content cannot be converted to T.
		*/
		template <typename T>
		T& cast()
		{
			if (small)
			{
				return *dynamic_cast<soo_any_model<T>&>(*reinterpret_cast<soo_any_concept*>(&m_value));
			}
			else
			{
				return *dynamic_cast<soo_any_model<T>&>(*m_value);
			}

		}

		/*!
		\brief Casts the held content to const T&.
		\tparam		Desired type.
		\returns	Returns a const reference to T.
		\throws		Throws std::bad_cast if the content cannot be converted to T.
		*/
		template <typename T>
		const T& cast() const
		{
			if (small)
			{
				return *dynamic_cast<soo_any_model<T>&>(*reinterpret_cast<soo_any_concept*>(&m_value));
			}
			else
			{
				return *dynamic_cast<soo_any_model<T>&>(*m_value);
			}
		}

		/*!
		\brief Implicit conversion to T&.
		*/
		template <typename T>
		operator T&()
		{
			return cast<T>();
		}

		/*!
		\brief Implicit conversion to const T&.
		*/
		template <typename T>
		operator const T&() const
		{
			return cast<T>();
		}

		/*!
		\brief Destroys the held object.
		*/
		void clear()
		{
			destruct_old();
			new(reinterpret_cast<void*>(&m_value))soo_any_model<int>(0);
			small = true;
		}


	private:
		bool small;
		union
		{
			soo_any_concept* m_value;
			unsigned char m_sob[ANY_SOB_SIZE];
		};
	};

	/*!
	\brief Implements the type erasure idiom with customizable maximum size for small object optimization.

	Small object optimization prevents heap allocations for object sizes up to the size specified
	with the SMALL_OBJECT_SIZE template parameter.

	\tparam SMALL_OBJECT_SIZE	Maximum object size up to which small object optimization works.
	*/
	template<size_t SMALL_OBJECT_SIZE = 16>
	class soo_any_s
	{
	private:
		//some size calc stuff

		using this_type = soo_any_s<SMALL_OBJECT_SIZE>;
		static constexpr size_t VPTR_SIZE = sizeof(void*); //hopefully msvc doesn't do weird stuff to vptr in the future
		static constexpr size_t BUF_SIZE = SMALL_OBJECT_SIZE + VPTR_SIZE;
		static constexpr size_t SO_SIZE = SMALL_OBJECT_SIZE;

		//CONCEPT
		struct soo_any_s_concept
		{
			using ptr = std::unique_ptr<soo_any_s_concept>;
			virtual ~soo_any_s_concept() {}
			virtual const std::type_info& type() const = 0;
			virtual soo_any_s_concept* clone() const = 0;
			virtual soo_any_s_concept* move() = 0;
			virtual soo_any_s_concept* placement_clone(void* ptr) const = 0;
			virtual soo_any_s_concept* placement_move(void* ptr) = 0;
			virtual size_t size() const = 0;
			virtual size_t internal_size() const = 0;
		};

		//MODEL
		template <typename T>
		struct soo_any_s_model : public soo_any_s_concept
		{
			soo_any_s_model(T _val) :
				value(std::move(_val))
			{}

			virtual const std::type_info & type() const override
			{
				return typeid(T);
			}

			virtual soo_any_s_concept * clone() const override
			{
				return new soo_any_s_model(value);
			}

			virtual soo_any_s_concept * move() override
			{
				return new soo_any_s_model(std::move(value));
			}

			virtual soo_any_s_concept * placement_clone(void* ptr) const override
			{
				return new(ptr)soo_any_s_model(value);
			}

			virtual soo_any_s_concept * placement_move(void* ptr) override
			{
				return new(ptr)soo_any_s_model(std::move(value));
			}

			virtual size_t size() const override
			{
				return sizeof(T);
			}

			virtual size_t internal_size() const override
			{
				return sizeof(soo_any_s_model<T>);
			}

			T& operator*()
			{
				return value;
			}

			const T& operator*() const
			{
				return value;
			}

		private:
			T value;
		};

	public:
		//PUBLIC INTERFACE
		//! Default constructor
		soo_any_s()
		{
			new(reinterpret_cast<void*>(&m_value))soo_any_s_model<int>(0);
			small = true;
			//m_value = nullptr;
		}

		~soo_any_s()
		{
			destruct_old();
		}

		void destruct_old()
		{
			if (small)
			{
				reinterpret_cast<soo_any_s_concept*>(&m_value)->~soo_any_s_concept();
				new(reinterpret_cast<void*>(&m_value))soo_any_s_model<int>(0);
				small = true;
			}
			else
			{
				delete m_value;
				new(reinterpret_cast<void*>(&m_value))soo_any_s_model<int>(0);
				small = true;
			}
		}

		/*!
		\brief Initializes the any object with some arbitrary data.
		\tparam T		Type of the data.
		\param[in] val	Data.
		*/
		template <typename T>
		soo_any_s(T val)
		{
			if (sizeof(soo_any_s_model<T>) > BUF_SIZE)
			{
				m_value = new soo_any_s_model<T>(std::forward<T>(val));
				small = false;
			}
			else
			{
				new(reinterpret_cast<void*>(&m_value))soo_any_s_model<T>(std::forward<T>(val));
				small = true;
			}
		}

		/*!
		\brief Assigns new data to the any object
		\tparam T		Type of the data.
		\param[in] val	Data.
		*/
		template <typename T>
		soo_any_s& operator=(T val)
		{
			destruct_old();
			if (sizeof(soo_any_s_model<T>) > BUF_SIZE)
			{
				m_value = new soo_any_s_model<T>(std::forward<T>(val));
				small = false;
			}
			else
			{
				new(reinterpret_cast<void*>(&m_value))soo_any_s_model<T>(std::forward<T>(val));
				small = true;
			}

			return *this;
			/*m_value = std::make_unique<soo_any_s_model<T>>(std::forward<T>(val));
			return *this;*/
		}

		/*!
		\brief Copy constructor.
		*/
		soo_any_s(const this_type& other)
		{
			if (other.small)
			{
				reinterpret_cast<const soo_any_s_concept*>(&other.m_value)->placement_clone(reinterpret_cast<void*>(&m_value));
				small = true;
			}
			else
			{
				m_value = other.m_value->clone();
				small = false;
			}
		}

		/*!
		\brief Move constructor.
		*/
		soo_any_s(this_type&& other)
		{
			if (other.small)
			{
				reinterpret_cast<soo_any_s_concept*>(&other.m_value)->placement_move(reinterpret_cast<void*>(&m_value));
				new(reinterpret_cast<void*>(&other.m_value))soo_any_s_model<int>(0);
				other.small = true;
				small = true;
			}
			else
			{
				m_value = other.m_value;
				new(reinterpret_cast<void*>(&other.m_value))soo_any_s_model<int>(0);
				other.small = true;
				small = false;
			}
		}

		/*!
		\brief Copy assignment.
		*/
		soo_any_s& operator=(const this_type& other)
		{
			if (this == &other)
				return *this;
			destruct_old();
			if (other.small)
			{
				reinterpret_cast<const soo_any_s_concept*>(&other.m_value)->placement_clone(reinterpret_cast<void*>(&m_value));
				small = true;
			}
			else
			{
				m_value = other.m_value->clone();
				small = false;
			}
			return *this;
		}

		/*!
		\brief Move assignment.
		*/
		soo_any_s& operator=(this_type&& other)
		{
			if (this == &other)
				return *this;
			destruct_old();
			if (other.small)
			{
				reinterpret_cast<soo_any_s_concept*>(&other.m_value)->placement_move(reinterpret_cast<void*>(&m_value));
				new(reinterpret_cast<void*>(&other.m_value))soo_any_s_model<int>(0);
				other.small = true;
				small = true;
			}
			else
			{
				m_value = other.m_value;
				new(reinterpret_cast<void*>(&other.m_value))soo_any_s_model<int>(0);
				other.small = true;
				small = false;
			}
			return *this;
		}

		/*!
		\brief Swaps content of two any's.
		*/
		void swap(this_type& other)
		{
			if (other.small)
			{
				if (small)
				{
					unsigned char tmp[BUF_SIZE];
					reinterpret_cast<soo_any_s_concept*>(&other.m_value)->placement_move(static_cast<void*>(tmp));
					reinterpret_cast<soo_any_s_concept*>(&m_value)->placement_move(static_cast<void*>(&other.m_value));
					reinterpret_cast<soo_any_s_concept*>(&tmp)->placement_move(static_cast<void*>(&m_value));
					reinterpret_cast<soo_any_s_concept*>(&tmp)->~soo_any_s_concept();
				}
				else
				{
					unsigned char tmp[BUF_SIZE];
					reinterpret_cast<soo_any_s_concept*>(&other.m_value)->placement_move(static_cast<void*>(tmp));
					other.small = false;
					other.m_value = m_value;
					reinterpret_cast<soo_any_s_concept*>(&tmp)->placement_move(static_cast<void*>(&m_value));
					reinterpret_cast<soo_any_s_concept*>(&tmp)->~soo_any_s_concept();
					small = true;
				}
			}
			else
			{
				if (small)
				{
					soo_any_s_concept* tmp = other.m_value;
					reinterpret_cast<soo_any_s_concept*>(&m_value)->placement_move(static_cast<void*>(&other.m_value));
					other.small = true;
					reinterpret_cast<soo_any_s_concept*>(&m_value)->~soo_any_s_concept();
					m_value = tmp;
					small = false;
				}
				else
				{
					soo_any_s_concept* tmp = other.m_value;
					other.m_value = m_value;
					m_value = tmp;
				}
			}
		}

		/*!
		\brief Returns the size of the held object.
		*/
		size_t size() const
		{
			if (small)
			{
				return reinterpret_cast<const soo_any_s_concept*>(&m_value)->size();
			}
			else
			{
				return m_value->size();
			}

		}

		/*!
		\brief returns std::type_info for the held object.
		*/
		const std::type_info& type() const
		{
			if (small)
			{
				return reinterpret_cast<const soo_any_s_concept*>(&m_value)->type();
			}
			else
			{
				return m_value->type();
			}
		}

		/*!
		\brief Casts the held content to T&.
		\tparam		Desired type.
		\returns	Returns a reference to T.
		\throws		Throws std::bad_cast if the content cannot be converted to T.
		*/
		template <typename T>
		T& cast()
		{
			if (small)
			{
				return *dynamic_cast<soo_any_s_model<T>&>(*reinterpret_cast<soo_any_s_concept*>(&m_value));
			}
			else
			{
				return *dynamic_cast<soo_any_s_model<T>&>(*m_value);
			}

		}

		/*!
		\brief Casts the held content to const T&.
		\tparam		Desired type.
		\returns	Returns a const reference to T.
		\throws		Throws std::bad_cast if the content cannot be converted to T.
		*/
		template <typename T>
		const T& cast() const
		{
			if (small)
			{
				return *dynamic_cast<soo_any_s_model<T>&>(*reinterpret_cast<soo_any_s_concept*>(&m_value));
			}
			else
			{
				return *dynamic_cast<soo_any_s_model<T>&>(*m_value);
			}
		}

		/*!
		\brief Implicit conversion to T&.
		*/
		template <typename T>
		operator T&()
		{
			return cast<T>();
		}

		/*!
		\brief Implicit conversion to const T&.
		*/
		template <typename T>
		operator const T&() const
		{
			return cast<T>();
		}

		/*!
		\brief Destroys the held object.
		*/
		void clear()
		{
			destruct_old();
			new(reinterpret_cast<void*>(&m_value))soo_any_s_model<int>(0);
			small = true;
		}


	private:		
		union
		{
			soo_any_s_concept* m_value;
			unsigned char m_sob[BUF_SIZE];
		};
		bool small;
	};
}
#endif
/** @}*/