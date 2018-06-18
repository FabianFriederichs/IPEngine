/** \addtogroup typelibrary
*  @{
*/

/*!
\file function.h
*/
#ifndef _FUNCTION_H_
#define _FUNCTION_H_
#include <exception>
//general purpose function wrapper
//define #define FUNCTION_SUPPORT_FUNCTORS to activate arbitrary functor support. 
//TODO: Small function initialization
#define FUNCTION_SUPPORT_FUNCTORS

namespace ipengine {

	//! Is thrown when an invalid function object is invoked
	class bad_invocation_exception : public std::exception
	{
	public:
		const char* what() const noexcept override
		{
			return "An empty function object was invoked.";
		}
	};

	/*!
	\brief unused
	*/
	template <class UNUSED_T>
	class function;

	/*!
	\brief Implements a general purpose function wrapper.

	In contrast to the std:: implementation, everything that is callable with the same signature
	is handled as the same type. This achieved through some void pointer- and template trickery.

	Complete objects with a function call operator are supported when FUNCTION_SUPPORT_FUNCTORS is defined.
	This is optional, because callable objects aka functors have to be allocated on the heap, because
	their size is not fixed, and heap allocations are slow. Small object optimization like the one in soo_any
	could be implemented, though.

	Instances of function are created by calling factory functions, which is more convenient than creating
	them with constructor calls, because some type deduction features are not available with constructors.

	\todo Implement small object optimization
	*/
	template <class R, class ... ARGS>
	class function<R(ARGS...)>
	{
	private:
		//internal types
		using func_ptr = R(*)(ARGS...);
		using moffunc_ptr = R(*)(void*, ARGS...);
#ifdef FUNCTION_SUPPORT_FUNCTORS
		//using functor_deleter = void(*)(void*);
		using functor_creator = void*(*)(void*);
		using functor_copy = void*(*)(void*);
#endif

		union FWrp
		{
			func_ptr function;
			moffunc_ptr moffunction;
		};

		//data
		void* object_ptr;
		FWrp func_store;

#ifdef FUNCTION_SUPPORT_FUNCTORS
		//functor_deleter ftdeleter;
		functor_creator ftcreator;	//used as both: creator and deleter
		functor_copy ftcopyctor;
#endif

		//memfunc stub
		template <class Class, R(Class::*mfptr)(ARGS...)>
		static R mfunc_stub(void* obj, ARGS... args)
		{
			return (static_cast<Class*>(obj)->*mfptr)(std::forward<ARGS>(args)...);
		}

		template <class Class, R(Class::*mfptr)(ARGS...) const>
		static R mfunc_stub(void* obj, ARGS... args)
		{
			return (static_cast<Class*>(obj)->*mfptr)(std::forward<ARGS>(args)...);
		}

#ifdef FUNCTION_SUPPORT_FUNCTORS
		//general functor stub
		template <class Functor>
		static R functor_stub(void* f, ARGS... args)
		{
			return (*static_cast<Functor*>(f))(std::forward<ARGS>(args)...);
		}

		//functor deleter stub
		/*template <class Functor>
		static void delete_functor(void* f)
		{
			delete static_cast<Functor*>(f);
		}*/

		//default constructor stub for Functor
		template <class Functor>
		static void* create_functor(void* ptr)
		{
			if (!ptr)
				return static_cast<void*>(new Functor());
			else
				delete ptr;
		}

		//copy and move constructor for Functor
		template <class Functor>
		static void* copy_functor(void* other)
		{
			return static_cast<void*>(new Functor(*static_cast<Functor*>(other)));
		}
#endif

		//used only for constructor readability. Do not touch elsewhere except you like memory leaks...
		void clear() noexcept
		{
			func_store.function = nullptr;
			object_ptr = nullptr;
#ifdef FUNCTION_SUPPORT_FUNCTORS
			//ftdeleter = nullptr;
			ftcreator = nullptr;
			ftcopyctor = nullptr;
#endif
		}

	public:
		//! Return type
		using return_type = R;

		//! Creates an empty function object.
		function() :
			func_store{ nullptr },
			object_ptr(nullptr)
#ifdef FUNCTION_SUPPORT_FUNCTORS
			,
			//ftdeleter(nullptr),
			ftcreator(nullptr),
			ftcopyctor(nullptr)
#endif

		{
		}

		//! Copy constructor
		function(const function<R(ARGS...)>& other) :
			func_store(other.func_store),
			object_ptr(other.object_ptr)
#ifdef FUNCTION_SUPPORT_FUNCTORS
			,
			//ftdeleter(other.ftdeleter),
			ftcreator(other.ftcreator),
			ftcopyctor(other.ftcopyctor)
#endif
		{
#ifdef FUNCTION_SUPPORT_FUNCTORS
			if (other.object_ptr != nullptr && other.ftcopyctor != nullptr)
			{
				object_ptr = (*ftcopyctor)(other.object_ptr);
			}
#endif
		}

		//! Move constructor
		function(function<R(ARGS...)>&& other) noexcept :
		func_store(other.func_store),
			object_ptr(other.object_ptr)
#ifdef FUNCTION_SUPPORT_FUNCTORS
			,
			//ftdeleter(other.ftdeleter),
			ftcreator(other.ftcreator),
			ftcopyctor(other.ftcopyctor)
#endif
		{
			other.clear();
		}

		//! Destructor
		~function()
		{
#ifdef FUNCTION_SUPPORT_FUNCTORS
			if (ftcreator != nullptr && object_ptr != nullptr)
			{
				(*ftcreator)(object_ptr);
			}
#endif
		}

		//! Copy assignment.
		function<R(ARGS...)>& operator=(const function<R(ARGS...)>& other)
		{
			if (this == &other)
				return *this;
			function<R(ARGS...)> newfunc(other);
			swap(newfunc);
			return *this;
		}

		//! Move assignment.
		function<R(ARGS...)>& operator=(function<R(ARGS...)>&& other) noexcept
		{
			if (this == &other)
				return *this;
			func_store = other.func_store;
			object_ptr = other.object_ptr;
#ifdef FUNCTION_SUPPORT_FUNCTORS
			//ftdeleter = other.ftdeleter;
			ftcreator = other.ftcreator;
			ftcopyctor = other.ftcopyctor;
#endif
			other.clear();
			return *this;
		}

		//! Swaps contents of two function objects
		void swap(function<R(ARGS...)>& other) noexcept
		{
			FWrp fstore = func_store;
			func_store = other.func_store;
			other.func_store = fstore;

			void* objptr = object_ptr;
			object_ptr = other.object_ptr;
			other.object_ptr = objptr;

#ifdef FUNCTION_SUPPORT_FUNCTORS
			/*functor_deleter fd = ftdeleter;
			ftdeleter = other.ftdeleter;
			other.ftdeleter = fd;*/

			functor_creator fc = ftcreator;
			ftcreator = other.ftcreator;
			other.ftcreator = fc;

			functor_copy fcp = ftcopyctor;
			ftcopyctor = other.ftcopyctor;
			other.ftcopyctor = fcp;
#endif
		}

		//factory
		//function pointers
		//compile time version
		/*!
		\brief Create a function object with a plain function pointer known at compile time.
		\tparam func	Function pointer
		\returns		function object
		*/
		template <func_ptr func>
		static function<R(ARGS...)> make_func()
		{
			function<R(ARGS...)> f;
			f.object_ptr = nullptr;
			f.func_store.function = func;
			return f;
		}

		/*!
		\brief Create a function object with a plain function pointer not known at compile time.
		\param func		Function pointer
		\returns		function object
		*/
		static function<R(ARGS...)> make_func(func_ptr func)
		{
			function<R(ARGS...)> f;
			f.object_ptr = nullptr;
			f.func_store.function = func;
			return f;
		}

		/*!
		\brief Create a function object that wraps a member function pointer.
		\tparam Class	Type of the class
		\tparam memfunc	Pointer to member function. Must be known at compile time.
		\returns		function object

		Example usage:

		Let there be a class:
		\code{.cpp}
		class Test
		{
		public:
			int somefunction(int, float);
		};
		\endcode

		A function object that wraps Test::somefunction is created as follows:
		\code{.cpp}
		Test t;
		auto testfunc = function<int(int, float)>::make_func<Test, &Test::somefunction>(&t);
		//invoke it
		int res = testfunc(42, 3.0f);
		\endcode

		\attention	Make sure that the object lives at least as long as the function object that references the memberfunction. Undefined behaviour otherwise.
		*/		
		template <class Class, R(Class::*memfunc)(ARGS...)>
		static function<R(ARGS...)> make_func(Class* object)
		{
			function<R(ARGS...)> f;
			f.object_ptr = static_cast<void*>(object);
			f.func_store.moffunction = &mfunc_stub<Class, memfunc>;
			return f;
		}

		/*!
		\brief Create a function object that wraps a const member function pointer.
		\tparam Class	Type of the class
		\tparam memfunc	Pointer to member function. Must be known at compile time.
		\returns		function object

		Example usage:

		Let there be a class:
		\code{.cpp}
		class Test
		{
		public:
		int somefunction(int, float) const;
		};
		\endcode

		A function object that wraps Test::somefunction is created as follows:
		\code{.cpp}
		Test t;
		auto testfunc = function<int(int, float)>::make_func<Test, &Test::somefunction>(&t);
		//invoke it
		int res = testfunc(42, 3.0f);
		\endcode

		\attention	Make sure that the object lives at least as long as the function object that references the memberfunction. Undefined behaviour otherwise.
		*/
		template <class Class, R(Class::*memfunc)(ARGS...) const>
		static function<R(ARGS...)> make_func(Class* object)
		{
			function<R(ARGS...)> f;
			f.object_ptr = static_cast<void*>(object);
			f.func_store.moffunction = &mfunc_stub<Class, memfunc>;
			return f;
		}

#ifdef FUNCTION_SUPPORT_FUNCTORS
		/*!
		\brief Create a function object from a callable object (functor).

		Note that functors are stored on the heap which adds additional cost to creation of the function object.

		\tparam Functor		Type of the functor.
		\param[in] func		Reference to an instance of Functor
		\returns			Returns a function object.
		*/
		template <class Functor>
		static function<R(ARGS...)> make_func(Functor&& func)
		{
			function<R(ARGS...)> f;
			f.object_ptr = static_cast<void*>(new std::remove_reference<Functor>::type(std::forward<Functor>(func)));
			//f.ftdeleter = &delete_functor<std::remove_reference<Functor>::type>;
			f.ftcreator = &create_functor<std::remove_reference<Functor>::type>;
			f.ftcopyctor = &copy_functor<std::remove_reference<Functor>::type>;
			f.func_store.moffunction = &functor_stub<std::remove_reference<Functor>::type>;
			return f;
		}
#endif
		/*!
		\brief Calls whatever is held by the function object with the given arguments.

		The arguments are perfect-forwarded to the called entity.

		\returns	Returns whatever the called entity returns.
		\throws		Throws bad_invocation_exception if the function object is invalid or empty.
		*/
		R operator()(ARGS&&... args)
		{
			if (func_store.function == nullptr)
				throw bad_invocation_exception();
			if (object_ptr == nullptr)
				return (*func_store.function)(std::forward<ARGS>(args)...);
			else
				return (*func_store.moffunction)(object_ptr, std::forward<ARGS>(args)...);
		}

		/*!
		\brief Returns true if the function object is empty.
		\returns	Returns true if the function object is empty.
		*/
		bool isEmpty()
		{
			return func_store.function == nullptr;
		}
	};

}
#endif
/** @}*/