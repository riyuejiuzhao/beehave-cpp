#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {
	template<typename Predicate>
	bool any_of(const TypedArray<Node> &arr, Predicate pred) {
		for (int i = 0; i < arr.size(); i++) {
			Node *node = Object::cast_to<Node>(arr[i]);
			if (node && pred(node)) {
				return true;
			}
		}
		return false;
	}

	template<typename Predicate>
	TypedArray<Node> filter(const TypedArray<Node> &arr, Predicate pred) {
		TypedArray<Node> out;
		for (int i = 0; i < arr.size(); i++) {
			Node *node = Object::cast_to<Node>(arr[i]);
			if (node && pred(node)) {
				out.push_back(node);
			}
		}
		return out;
	}

	template<typename Predicate>
	int count_if(const TypedArray<Node> &arr, Predicate pred) {
		int count = 0;
		for (int i = 0; i < arr.size(); i++) {
			Node *node = Object::cast_to<Node>(arr[i]);
			if (node && pred(node)) {
				count++;
			}
		}
		return count;
	}

} // namespace godot
