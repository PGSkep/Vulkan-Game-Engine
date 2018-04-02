#ifndef	MEMORY_MAP_H
#define MEMORY_MAP_H

#include <map>
#include "../../Common/Utility/Error.h"

template <class Userdata>
class MemoryMap
{
	struct MemoryNode
	{
		bool free;
		size_t offset;
		size_t size;
		Userdata userdata;

		MemoryNode* prev;
		MemoryNode* next;
	};
	std::map<std::string, MemoryNode*> memoryMap;

	MemoryNode* head;
	size_t size = 0;

public:
	void Init(size_t _size);
	size_t Allocate(const char* _name, size_t _size, Userdata _userdata);
	void Free(const char* _name);
	struct MemoryInfo
	{
		size_t offset;
		size_t size;
		Userdata* userdataPtr;
	};
	MemoryInfo Get(const char* _name)
	{
		std::map<std::string, MemoryNode*>::iterator iter = memoryMap.find(_name);
		if (iter == memoryMap.end())
			return {};
		
		MemoryInfo info;
		info.offset = iter->second->offset;
		info.size = iter->second->size;
		info.userdataPtr = &iter->second->userdata;
		return info;
	}
	void ShutDown();

	typename typedef std::map<std::string, MemoryNode*>::iterator Iterator;
	Iterator GetBegin()
	{
		Iterator iterator;
		iterator = memoryMap.begin();
		return iterator;
	}
	Iterator GetEnd()
	{
		Iterator iterator;
		iterator = memoryMap.end();
		return iterator;
	}
};

template <class Userdata>
void MemoryMap<Userdata>::Init(size_t _size)
{
	size = _size;

	head = new MemoryNode;
	head->prev = nullptr;
	head->next = nullptr;
	head->free = true;
	head->offset = 0;
	head->size = size;
	head->userdata = {};
}

template <class Userdata>
size_t MemoryMap<Userdata>::Allocate(const char* _name, size_t _size, Userdata _userdata)
{
	MemoryNode* curr = head;
	MemoryNode* target = nullptr;

	while (curr != nullptr)
	{
		if (curr->free)
		{
			if (curr->size >= _size)
			{
				target = curr;
				break;
			}
		}
		else
			curr = curr->next;
	}

	if (target == nullptr)
	{
		Error::Report("Failed to allocate, could not find empty space with fitting size");
		return -1;
	}
	else
	{
		target->free = false;

		if (target->size != _size)
		{
			MemoryNode* newNode = new MemoryNode;
			newNode->offset = target->offset + _size;
			newNode->size = target->size - _size;
			newNode->userdata = {};

			target->size -= newNode->size;
			target->userdata = _userdata;

			newNode->prev = target;
			newNode->next = target->next;
			if(target->next != nullptr)
				target->next->prev = newNode;
			target->next = newNode;
		}

		memoryMap[_name] = target;

		return target->offset;
	}
}

template <class Userdata>
void MemoryMap<Userdata>::Free(const char* _name)
{
	std::map<std::string, MemoryNode*>::iterator search = memoryMap.find(_name);

	if (search == memoryMap.end())
		Error::Report("Could not find memory to free");

	if (search->second->free == true)
		Error::Report("Memory is free");

	// merge next free memory
	if (search->second->next != nullptr)
	{
		if (search->second->next->free == true)
		{
			MemoryNode* temp = search->second->next;
			search->second->size += temp->size;

			search->second->next = temp->next;
			if (temp->next != nullptr)
				temp->next->prev = search->second;
			delete temp;

			memoryMap.erase(++search);
		}
	}

	// merge prev free memory
	if (search->second->prev != nullptr)
	{
		if (search->second->prev->free == true)
		{
			MemoryNode* temp = search->second->prev;
			search->second->size += temp->size;

			search->second->prev = temp->prev;
			if (temp->prev != nullptr)
				temp->prev->next = search->second;
			delete temp;

			memoryMap.erase(--search);
		}
	}

	search->second->free = true;
	memoryMap.erase(search);
}

template <class Userdata>
void MemoryMap<Userdata>::ShutDown()
{
	MemoryNode* temp;

	while (head != nullptr)
	{
		temp = head;
		head = head->next;

		delete temp;
	}
}

#endif