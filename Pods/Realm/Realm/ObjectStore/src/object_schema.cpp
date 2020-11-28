////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#include "object_schema.hpp"

#include "feature_checks.hpp"
#include "object_store.hpp"
#include "property.hpp"
#include "schema.hpp"

<<<<<<< HEAD

#include <realm/data_type.hpp>
#include <realm/descriptor.hpp>
=======
#include <realm/data_type.hpp>
>>>>>>> origin/develop12
#include <realm/group.hpp>
#include <realm/table.hpp>

using namespace realm;

ObjectSchema::ObjectSchema() = default;
ObjectSchema::~ObjectSchema() = default;

ObjectSchema::ObjectSchema(std::string name, std::initializer_list<Property> persisted_properties)
: ObjectSchema(std::move(name), persisted_properties, {})
{
}

ObjectSchema::ObjectSchema(std::string name, std::initializer_list<Property> persisted_properties,
                           std::initializer_list<Property> computed_properties)
: name(std::move(name))
, persisted_properties(persisted_properties)
, computed_properties(computed_properties)
{
    for (auto const& prop : persisted_properties) {
        if (prop.is_primary) {
            primary_key = prop.name;
            break;
        }
    }
}

<<<<<<< HEAD
PropertyType ObjectSchema::from_core_type(Descriptor const& table, size_t col)
{
    auto optional = table.is_nullable(col) ? PropertyType::Nullable : PropertyType::Required;
    switch (table.get_column_type(col)) {
        case type_Int:       return PropertyType::Int | optional;
        case type_Float:     return PropertyType::Float | optional;
        case type_Double:    return PropertyType::Double | optional;
        case type_Bool:      return PropertyType::Bool | optional;
        case type_String:    return PropertyType::String | optional;
        case type_Binary:    return PropertyType::Data | optional;
        case type_Timestamp: return PropertyType::Date | optional;
        case type_Mixed:     return PropertyType::Any | optional;
        case type_Link:      return PropertyType::Object | PropertyType::Nullable;
        case type_LinkList:  return PropertyType::Object | PropertyType::Array;
        case type_Table:     return from_core_type(*table.get_subdescriptor(col), 0) | PropertyType::Array;
=======
PropertyType ObjectSchema::from_core_type(Table const& table, ColKey col)
{
    auto flags = PropertyType::Required;
    auto attr = table.get_column_attr(col);
    if (attr.test(col_attr_Nullable))
        flags |= PropertyType::Nullable;
    if (attr.test(col_attr_List))
        flags |= PropertyType::Array;
    switch (table.get_column_type(col)) {
        case type_Int:       return PropertyType::Int | flags;
        case type_Float:     return PropertyType::Float | flags;
        case type_Double:    return PropertyType::Double | flags;
        case type_Bool:      return PropertyType::Bool | flags;
        case type_String:    return PropertyType::String | flags;
        case type_Binary:    return PropertyType::Data | flags;
        case type_Timestamp: return PropertyType::Date | flags;
        case type_OldMixed:  return PropertyType::Any | flags;
        case type_Link:      return PropertyType::Object | PropertyType::Nullable;
        case type_LinkList:  return PropertyType::Object | PropertyType::Array;
>>>>>>> origin/develop12
        default: REALM_UNREACHABLE();
    }
}

<<<<<<< HEAD
ObjectSchema::ObjectSchema(Group const& group, StringData name, size_t index) : name(name) {
    ConstTableRef table;
    if (index < group.size()) {
        table = group.get_table(index);
=======
ObjectSchema::ObjectSchema(Group const& group, StringData name, TableKey key)
: name(name)
{
    ConstTableRef table;
    if (key) {
        table = group.get_table(key);
>>>>>>> origin/develop12
    }
    else {
        table = ObjectStore::table_for_object_type(group, name);
    }
<<<<<<< HEAD

    size_t count = table->get_column_count();
    persisted_properties.reserve(count);
    for (size_t col = 0; col < count; col++) {
        if (auto property = ObjectStore::property_for_column_index(table, col)) {
            persisted_properties.push_back(std::move(property.value()));
        }
    }

    primary_key = realm::ObjectStore::get_primary_key_for_object(group, name);
    set_primary_key_property();
}

Property *ObjectSchema::property_for_name(StringData name)
=======
    table_key = table->get_key();

    size_t count = table->get_column_count();
    persisted_properties.reserve(count);

    for (auto col_key : table->get_column_keys()) {
        StringData column_name = table->get_column_name(col_key);

#if REALM_ENABLE_SYNC
        // The object ID column is an implementation detail, and is omitted from the schema.
        // FIXME: this can go away once sync adopts stable ids?
        if (column_name.begins_with("!"))
            continue;
#endif

        Property property;
        property.name = column_name;
        property.type = ObjectSchema::from_core_type(*table, col_key);
        property.is_indexed = table->has_search_index(col_key) || table->get_primary_key_column() == col_key;
        property.column_key = col_key;

        if (property.type == PropertyType::Object) {
            // set link type for objects and arrays
            ConstTableRef linkTable = table->get_link_target(col_key);
            property.object_type = ObjectStore::object_type_for_table_name(linkTable->get_name().data());
        }
        persisted_properties.push_back(std::move(property));
    }

    primary_key = ObjectStore::get_primary_key_for_object(group, name);
    set_primary_key_property();
}

Property *ObjectSchema::property_for_name(StringData name) noexcept
>>>>>>> origin/develop12
{
    for (auto& prop : persisted_properties) {
        if (StringData(prop.name) == name) {
            return &prop;
        }
    }
    for (auto& prop : computed_properties) {
        if (StringData(prop.name) == name) {
            return &prop;
        }
    }
    return nullptr;
}

<<<<<<< HEAD
Property *ObjectSchema::property_for_public_name(StringData public_name)
=======
Property *ObjectSchema::property_for_public_name(StringData public_name) noexcept
>>>>>>> origin/develop12
{
    // If no `public_name` is defined, the internal `name` is also considered the public name.
    for (auto& prop : persisted_properties) {
        if (prop.public_name == public_name || (prop.public_name.empty() && prop.name == public_name))
            return &prop;
    }

    // Computed properties are not persisted, so creating a public name for such properties
    // are a bit pointless since the internal name is already the "public name", but since
    // this distinction isn't visible in the Property struct we allow it anyway.
    for (auto& prop : computed_properties) {
<<<<<<< HEAD
        if ((prop.public_name.empty() ? StringData(prop.name) :  StringData(prop.public_name)) == public_name)
=======
        if (StringData(prop.public_name.empty() ? prop.name : prop.public_name) == public_name)
>>>>>>> origin/develop12
            return &prop;
    }
    return nullptr;
}

<<<<<<< HEAD
const Property *ObjectSchema::property_for_public_name(StringData public_name) const
=======
const Property *ObjectSchema::property_for_public_name(StringData public_name) const noexcept
>>>>>>> origin/develop12
{
    return const_cast<ObjectSchema *>(this)->property_for_public_name(public_name);
}

<<<<<<< HEAD
const Property *ObjectSchema::property_for_name(StringData name) const
=======
const Property *ObjectSchema::property_for_name(StringData name) const noexcept
>>>>>>> origin/develop12
{
    return const_cast<ObjectSchema *>(this)->property_for_name(name);
}

<<<<<<< HEAD
bool ObjectSchema::property_is_computed(Property const& property) const
=======
bool ObjectSchema::property_is_computed(Property const& property) const noexcept
>>>>>>> origin/develop12
{
    auto end = computed_properties.end();
    return std::find(computed_properties.begin(), end, property) != end;
}

<<<<<<< HEAD
void ObjectSchema::set_primary_key_property()
=======
void ObjectSchema::set_primary_key_property() noexcept
>>>>>>> origin/develop12
{
    if (primary_key.length()) {
        if (auto primary_key_prop = primary_key_property()) {
            primary_key_prop->is_primary = true;
        }
    }
}

static void validate_property(Schema const& schema,
                              std::string const& object_name,
                              Property const& prop,
                              Property const** primary,
                              std::vector<ObjectSchemaValidationException>& exceptions)
{
    if (prop.type == PropertyType::LinkingObjects && !is_array(prop.type)) {
        exceptions.emplace_back("Linking Objects property '%1.%2' must be an array.",
                                object_name, prop.name);
    }

    // check nullablity
    if (is_nullable(prop.type) && !prop.type_is_nullable()) {
        exceptions.emplace_back("Property '%1.%2' of type '%3' cannot be nullable.",
                                object_name, prop.name, string_for_property_type(prop.type));
    }
    else if (prop.type == PropertyType::Object && !is_nullable(prop.type) && !is_array(prop.type)) {
        exceptions.emplace_back("Property '%1.%2' of type 'object' must be nullable.", object_name, prop.name);
    }

    // check primary keys
    if (prop.is_primary) {
        if (prop.type != PropertyType::Int && prop.type != PropertyType::String) {
            exceptions.emplace_back("Property '%1.%2' of type '%3' cannot be made the primary key.",
                                    object_name, prop.name, string_for_property_type(prop.type));
        }
        if (*primary) {
            exceptions.emplace_back("Properties '%1' and '%2' are both marked as the primary key of '%3'.",
                                    prop.name, (*primary)->name, object_name);
        }
        *primary = &prop;
    }

    // check indexable
    if (prop.is_indexed && !prop.type_is_indexable()) {
        exceptions.emplace_back("Property '%1.%2' of type '%3' cannot be indexed.",
                                object_name, prop.name, string_for_property_type(prop.type));
    }

    // check that only link properties have object types
    if (prop.type != PropertyType::LinkingObjects && !prop.link_origin_property_name.empty()) {
        exceptions.emplace_back("Property '%1.%2' of type '%3' cannot have an origin property name.",
                                object_name, prop.name, string_for_property_type(prop.type));
    }
    else if (prop.type == PropertyType::LinkingObjects && prop.link_origin_property_name.empty()) {
        exceptions.emplace_back("Property '%1.%2' of type '%3' must have an origin property name.",
                                object_name, prop.name, string_for_property_type(prop.type));
    }

    if (prop.type != PropertyType::Object && prop.type != PropertyType::LinkingObjects) {
        if (!prop.object_type.empty()) {
            exceptions.emplace_back("Property '%1.%2' of type '%3' cannot have an object type.",
                                    object_name, prop.name, prop.type_string());
        }
        return;
    }


    // check that the object_type is valid for link properties
    auto it = schema.find(prop.object_type);
    if (it == schema.end()) {
        exceptions.emplace_back("Property '%1.%2' of type '%3' has unknown object type '%4'",
                                object_name, prop.name, string_for_property_type(prop.type), prop.object_type);
        return;
    }
    if (prop.type != PropertyType::LinkingObjects) {
        return;
    }

    const Property *origin_property = it->property_for_name(prop.link_origin_property_name);
    if (!origin_property) {
        exceptions.emplace_back("Property '%1.%2' declared as origin of linking objects property '%3.%4' does not exist",
                                prop.object_type, prop.link_origin_property_name,
                                object_name, prop.name);
    }
    else if (origin_property->type != PropertyType::Object) {
        exceptions.emplace_back("Property '%1.%2' declared as origin of linking objects property '%3.%4' is not a link",
                                prop.object_type, prop.link_origin_property_name,
                                object_name, prop.name);
    }
    else if (origin_property->object_type != object_name) {
        exceptions.emplace_back("Property '%1.%2' declared as origin of linking objects property '%3.%4' links to type '%5'",
                                prop.object_type, prop.link_origin_property_name,
                                object_name, prop.name, origin_property->object_type);
    }
}

void ObjectSchema::validate(Schema const& schema, std::vector<ObjectSchemaValidationException>& exceptions) const
{
    std::vector<StringData> public_property_names;
    std::vector<StringData> internal_property_names;
    internal_property_names.reserve(persisted_properties.size() + computed_properties.size());
    auto gather_names = [&](auto const &properties) {
        for (auto const &prop : properties) {
            internal_property_names.push_back(prop.name);
            if (!prop.public_name.empty())
                public_property_names.push_back(prop.public_name);
        }
    };
    gather_names(persisted_properties);
    gather_names(computed_properties);
    std::sort(public_property_names.begin(), public_property_names.end());
    std::sort(internal_property_names.begin(), internal_property_names.end());

    // Check that property names and aliases are unique
    auto for_each_duplicate = [](auto &&container, auto &&fn) {
        auto end = container.end();
        for (auto it = std::adjacent_find(container.begin(), end); it != end; it = std::adjacent_find(it + 2, end))
            fn(*it);
    };
    for_each_duplicate(public_property_names, [&](auto public_property_name) {
        exceptions.emplace_back("Alias '%1' appears more than once in the schema for type '%2'.",
                                public_property_name, name);
    });
    for_each_duplicate(internal_property_names, [&](auto internal_name) {
        exceptions.emplace_back("Property '%1' appears more than once in the schema for type '%2'.",
                                internal_name, name);
    });

    // Check that no aliases conflict with property names
    struct ErrorWriter {
        ObjectSchema const &os;
        std::vector<ObjectSchemaValidationException> &exceptions;

        struct Proxy {
            ObjectSchema const &os;
            std::vector<ObjectSchemaValidationException> &exceptions;

            Proxy &operator=(StringData name) {
                exceptions.emplace_back(
                        "Property '%1.%2' has an alias '%3' that conflicts with a property of the same name.",
                        os.name, os.property_for_public_name(name)->name, name);
                return *this;
            }
        };

        Proxy operator*() { return Proxy{os, exceptions}; }
        ErrorWriter &operator=(const ErrorWriter &) { return *this; }
        ErrorWriter &operator++() { return *this; }
        ErrorWriter &operator++(int) { return *this; }
    } writer{*this, exceptions};
    std::set_intersection(public_property_names.begin(), public_property_names.end(),
                          internal_property_names.begin(), internal_property_names.end(), writer);

    // Validate all properties
    const Property *primary = nullptr;
    for (auto const& prop : persisted_properties) {
        validate_property(schema, name, prop, &primary, exceptions);
    }
    for (auto const& prop : computed_properties) {
        validate_property(schema, name, prop, &primary, exceptions);
    }

    if (!primary_key.empty() && !primary && !primary_key_property()) {
        exceptions.emplace_back("Specified primary key '%1.%2' does not exist.", name, primary_key);
    }
}

namespace realm {
<<<<<<< HEAD
bool operator==(ObjectSchema const& a, ObjectSchema const& b)
=======
bool operator==(ObjectSchema const& a, ObjectSchema const& b) noexcept
>>>>>>> origin/develop12
{
    return std::tie(a.name, a.primary_key, a.persisted_properties, a.computed_properties)
        == std::tie(b.name, b.primary_key, b.persisted_properties, b.computed_properties);

}
}
