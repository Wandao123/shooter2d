#ifndef SINGLETON_H
#define SINGLETON_H

#include <cassert>
#include <mutex>

namespace Shooter {
	/// <summary>シングルトンクラスの共通処理。</summary>
	/// <remarks>コンストラクタの隠蔽などは継承先でも行う必要がある。</remarks>
	template <class T>
	class Singleton
	{
	public:
		static T& Create()
		{
			std::call_once(doesExists, []() { instance = new T; });
			assert(instance);
			return *instance;
		}

		static void Destroy()
		{
			delete instance;
			instance = nullptr;
		}

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator=(const Singleton&&) = delete;
	protected:
		Singleton() = default;
		virtual ~Singleton() = default;
	private:
		static std::once_flag doesExists;
		static T* instance;  // unique_ptr側からコンストラクタとデストラクタにアクセスできないので、生ポインタを使う。
	};

	template<typename T> std::once_flag Singleton<T>::doesExists;
	template<typename T> T* Singleton<T>::instance = nullptr;
}

#endif // !SINGLETON_H
