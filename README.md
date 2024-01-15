# Tentacle

Inversion of Control (Dependency Injection) framework for Unreal Engine

## Philosophy

This Plugin wants to help you stick to the **SOLID** principles while building your Games in Unreal Engine.
The solid principles are:

1. Single Responsibility
2. Open-Close
3. Liskov Substitution
4. Interface segregation
5. Dependency Inversion

Therefore this plugin provides Dependency Injection that supports Interfaces (**Interface Segregation**). We are a bit
opinionated towards composition over inheritance to avoid **Liskov Substitution** mistakes as much as possible
but inheritance is still supported.
Dependency implementations may be overridden by owning objects (**Dependency Inversion**) to change effective behaviour
without changing the inner logic of a class to achieve **Open Close**.
**Single Responsibility** is still up to you to adhere to :P

## Examples

