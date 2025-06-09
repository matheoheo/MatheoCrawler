#pragma once
#include <type_traits>
#include <SFML/Graphics.hpp>

struct IEvent;
struct IComponent;
struct AnimationFrame;
struct GenericAnimationKey;
struct EntityAnimationKey;
//This file contains definitions of different concepts with explaination about each.

//Makes sure that provided template parametr is an enum.
template <typename T>
concept EnumKey = std::is_enum_v<T>;

//Makes Sure that provided template parametr is an SFML Asset.
template <typename T>
concept SFMLAsset = std::is_same_v<T, sf::Texture> || std::is_same_v<T, sf::Font>;

template <typename T>
concept GameEventType = std::derived_from<T, IEvent>;

template <typename T>
concept ComponentType = std::derived_from<T, IComponent>;

//Constrains provided template parametr to be one of 2 types, animationKeys are either GenericAnimationKey or EntityAnimationKey
template <typename T>
concept AnimationKeyType = std::is_same_v<T, GenericAnimationKey> || std::is_same_v<T, EntityAnimationKey>;
