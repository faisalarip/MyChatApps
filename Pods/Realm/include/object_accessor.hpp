////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 Realm Inc.
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

#ifndef REALM_OS_OBJECT_ACCESSOR_HPP
#define REALM_OS_OBJECT_ACCESSOR_HPP

#include "object.hpp"

#include "feature_checks.hpp"
#include "list.hpp"
#include "object_schema.hpp"
#include "object_store.hpp"
#include "results.hpp"
#include "schema.hpp"
#include "shared_realm.hpp"

<<<<<<< HEAD
#include <realm/link_view.hpp>
=======
>>>>>>> origin/develop12
#include <realm/util/assert.hpp>
#include <realm/table_view.hpp>

#if REALM_ENABLE_SYNC
#include <realm/sync/object.hpp>
#endif // REALM_ENABLE_SYNC

#include <string>

namespace realm {
template <typename ValueType, typename ContextType>
<<<<<<< HEAD
void Object::set_property_value(ContextType& ctx, StringData prop_name, ValueType value, CreatePolicy policy)
{
    verify_attached();
    m_realm->verify_in_write();
    auto& property = property_for_name(prop_name);

    // Modifying primary keys is allowed in migrations to make it possible to
    // add a new primary key to a type (or change the property type), but it
    // is otherwise considered the immutable identity of the row
    if (property.is_primary && !m_realm->is_in_migration())
        throw ModifyPrimaryKeyException(m_object_schema->name, property.name);

=======
void Object::set_property_value(ContextType& ctx, StringData prop_name,
                                ValueType value, CreatePolicy policy)
{
    auto& property = property_for_name(prop_name);
    validate_property_for_setter(property);
>>>>>>> origin/develop12
    set_property_value_impl(ctx, property, value, policy, false);
}

template <typename ValueType, typename ContextType>
<<<<<<< HEAD
ValueType Object::get_property_value(ContextType& ctx, const Property& property)
=======
ValueType Object::get_property_value(ContextType& ctx, const Property& property) const
>>>>>>> origin/develop12
{
    return get_property_value_impl<ValueType>(ctx, property);
}

template <typename ValueType, typename ContextType>
<<<<<<< HEAD
ValueType Object::get_property_value(ContextType& ctx, StringData prop_name)
=======
ValueType Object::get_property_value(ContextType& ctx, StringData prop_name) const
>>>>>>> origin/develop12
{
    return get_property_value_impl<ValueType>(ctx, property_for_name(prop_name));
}

namespace {
template <typename ValueType, typename ContextType>
struct ValueUpdater {
    ContextType& ctx;
    Property const& property;
    ValueType& value;
<<<<<<< HEAD
    RowExpr row;
    size_t col;
    CreatePolicy policy;
    bool is_default;

    void operator()(RowExpr*)
    {
        ContextType child_ctx(ctx, property);
        auto curr_link = row.get_link(col);
        auto link = child_ctx.template unbox<RowExpr>(value, policy, curr_link);
        if (policy != CreatePolicy::UpdateModified || curr_link != link.get_index()) {
            row.set_link(col, link.get_index());
=======
    Obj& obj;
    ColKey col;
    CreatePolicy policy;
    bool is_default;

    void operator()(Obj*)
    {
        ContextType child_ctx(ctx, property);
        auto curr_link = obj.get<ObjKey>(col);
        auto link = child_ctx.template unbox<Obj>(value, policy, curr_link);
        if (policy != CreatePolicy::UpdateModified || curr_link != link.get_key()) {
            obj.set(col, link.get_key());
>>>>>>> origin/develop12
        }
    }

    template<typename T>
    void operator()(T*)
    {
        auto new_val = ctx.template unbox<T>(value);
<<<<<<< HEAD
        if (policy != CreatePolicy::UpdateModified || row.get<T>(col) != new_val) {
            row.set(col, new_val, is_default);
=======
        if (policy != CreatePolicy::UpdateModified || obj.get<T>(col) != new_val) {
            obj.set(col, new_val, is_default);
>>>>>>> origin/develop12
        }
    }
};
}

template <typename ValueType, typename ContextType>
void Object::set_property_value_impl(ContextType& ctx, const Property &property,
                                     ValueType value, CreatePolicy policy, bool is_default)
{
    ctx.will_change(*this, property);

<<<<<<< HEAD
    auto& table = *m_row.get_table();
    size_t col = property.table_column;
    size_t row = m_row.get_index();
    if (is_nullable(property.type) && ctx.is_null(value)) {
        if (policy != CreatePolicy::UpdateModified || !table.is_null(col, row)) {
            if (property.type == PropertyType::Object) {
                if (!is_default)
                    table.nullify_link(col, row);
            }
            else {
                table.set_null(col, row, is_default);
=======
    ColKey col{property.column_key};
    if (is_nullable(property.type) && ctx.is_null(value)) {
        if (policy != CreatePolicy::UpdateModified || !m_obj.is_null(col)) {
            if (property.type == PropertyType::Object) {
                if (!is_default)
                    m_obj.set_null(col);
            }
            else {
                m_obj.set_null(col, is_default);
>>>>>>> origin/develop12
            }
        }

        ctx.did_change();
        return;
    }

    if (is_array(property.type)) {
        if (property.type == PropertyType::LinkingObjects)
            throw ReadOnlyPropertyException(m_object_schema->name, property.name);

        ContextType child_ctx(ctx, property);
<<<<<<< HEAD
        List list(m_realm, table, col, m_row.get_index());
=======
        List list(m_realm, m_obj, col);
>>>>>>> origin/develop12
        list.assign(child_ctx, value, policy);
        ctx.did_change();
        return;
    }

    ValueUpdater<ValueType, ContextType> updater{ctx, property, value,
<<<<<<< HEAD
        table.get(row),col, policy, is_default};
=======
        m_obj, col, policy, is_default};
>>>>>>> origin/develop12
    switch_on_type(property.type, updater);
    ctx.did_change();
}

template <typename ValueType, typename ContextType>
<<<<<<< HEAD
ValueType Object::get_property_value_impl(ContextType& ctx, const Property &property)
{
    verify_attached();

    size_t column = property.table_column;
    if (is_nullable(property.type) && m_row.is_null(column))
        return ctx.null_value();
    if (is_array(property.type) && property.type != PropertyType::LinkingObjects)
        return ctx.box(List(m_realm, *m_row.get_table(), column, m_row.get_index()));

    switch (property.type & ~PropertyType::Flags) {
        case PropertyType::Bool:   return ctx.box(m_row.get_bool(column));
        case PropertyType::Int:    return ctx.box(m_row.get_int(column));
        case PropertyType::Float:  return ctx.box(m_row.get_float(column));
        case PropertyType::Double: return ctx.box(m_row.get_double(column));
        case PropertyType::String: return ctx.box(m_row.get_string(column));
        case PropertyType::Data:   return ctx.box(m_row.get_binary(column));
        case PropertyType::Date:   return ctx.box(m_row.get_timestamp(column));
        case PropertyType::Any:    return ctx.box(m_row.get_mixed(column));
        case PropertyType::Object: {
            auto linkObjectSchema = m_realm->schema().find(property.object_type);
            TableRef table = ObjectStore::table_for_object_type(m_realm->read_group(), property.object_type);
            return ctx.box(Object(m_realm, *linkObjectSchema, table->get(m_row.get_link(column))));
=======
ValueType Object::get_property_value_impl(ContextType& ctx, const Property &property) const
{
    verify_attached();

    ColKey column{property.column_key};
    if (is_nullable(property.type) && m_obj.is_null(column))
        return ctx.null_value();
    if (is_array(property.type) && property.type != PropertyType::LinkingObjects)
        return ctx.box(List(m_realm, m_obj, column));

    switch (property.type & ~PropertyType::Flags) {
        case PropertyType::Bool:   return ctx.box(m_obj.get<bool>(column));
        case PropertyType::Int:    return is_nullable(property.type) ? ctx.box(*m_obj.get<util::Optional<int64_t>>(column)) : ctx.box(m_obj.get<int64_t>(column));
        case PropertyType::Float:  return ctx.box(m_obj.get<float>(column));
        case PropertyType::Double: return ctx.box(m_obj.get<double>(column));
        case PropertyType::String: return ctx.box(m_obj.get<StringData>(column));
        case PropertyType::Data:   return ctx.box(m_obj.get<BinaryData>(column));
        case PropertyType::Date:   return ctx.box(m_obj.get<Timestamp>(column));
//        case PropertyType::Any:    return ctx.box(m_obj.get<Mixed>(column));
        case PropertyType::Object: {
            auto linkObjectSchema = m_realm->schema().find(property.object_type);
            return ctx.box(Object(m_realm, *linkObjectSchema,
                                  const_cast<Obj&>(m_obj).get_linked_object(column)));
>>>>>>> origin/develop12
        }
        case PropertyType::LinkingObjects: {
            auto target_object_schema = m_realm->schema().find(property.object_type);
            auto link_property = target_object_schema->property_for_name(property.link_origin_property_name);
<<<<<<< HEAD
            TableRef table = ObjectStore::table_for_object_type(m_realm->read_group(), target_object_schema->name);
            auto tv = m_row.get_table()->get_backlink_view(m_row.get_index(), table.get(), link_property->table_column);
=======
            auto table = m_realm->read_group().get_table(target_object_schema->table_key);
            auto tv = const_cast<Obj&>(m_obj).get_backlink_view(table, ColKey(link_property->column_key));
>>>>>>> origin/develop12
            return ctx.box(Results(m_realm, std::move(tv)));
        }
        default: REALM_UNREACHABLE();
    }
}

template<typename ValueType, typename ContextType>
Object Object::create(ContextType& ctx, std::shared_ptr<Realm> const& realm,
                      StringData object_type, ValueType value,
<<<<<<< HEAD
                      CreatePolicy policy, size_t current_row, Row* out_row)
{
    auto object_schema = realm->schema().find(object_type);
    REALM_ASSERT(object_schema != realm->schema().end());
    return create(ctx, realm, *object_schema, value, policy, current_row, out_row);
=======
                      CreatePolicy policy, ObjKey current_obj, Obj* out_row)
{
    auto object_schema = realm->schema().find(object_type);
    REALM_ASSERT(object_schema != realm->schema().end());
    return create(ctx, realm, *object_schema, value, policy, current_obj, out_row);
}

template<typename ValueType, typename ContextType>
Mixed as_mixed(ContextType& ctx, ValueType& value, PropertyType type)
{
    if (!value)
        return {};
    return switch_on_type(type, [&](auto* t) {
        return Mixed(ctx.template unbox<NonObjTypeT<decltype(*t)>>(*value));
    });
>>>>>>> origin/develop12
}

template<typename ValueType, typename ContextType>
Object Object::create(ContextType& ctx, std::shared_ptr<Realm> const& realm,
                      ObjectSchema const& object_schema, ValueType value,
<<<<<<< HEAD
                      CreatePolicy policy, size_t current_row, Row* out_row)
{
    realm->verify_in_write();

    // get or create our accessor
    bool created = false;

    // try to get existing row if updating
    size_t row_index = realm::not_found;
    TableRef table = ObjectStore::table_for_object_type(realm->read_group(), object_schema.name);

    bool skip_primary = true;
    if (auto primary_prop = object_schema.primary_key_property()) {
        // search for existing object based on primary key type
=======
                      CreatePolicy policy, ObjKey current_obj, Obj* out_row)
{
    realm->verify_in_write();

    // When setting each property, we normally want to skip over the primary key
    // as that's set as part of object creation. However, during migrations the
    // property marked as the primary key in the schema may not currently be
    // considered a primary key by core, and so will need to be set.
    bool skip_primary = true;
    // If the input value is missing values for any of the properties we want to
    // set the propery to the default value for new objects, but leave it
    // untouched for existing objects.
    bool created = false;

    Obj obj;
    auto table = realm->read_group().get_table(object_schema.table_key);

    // If there's a primary key, we need to first check if an object with the
    // same primary key already exists. If it does, we either update that object
    // or throw an exception if updating is disabled.
    if (auto primary_prop = object_schema.primary_key_property()) {
>>>>>>> origin/develop12
        auto primary_value = ctx.value_for_property(value, *primary_prop,
                                                    primary_prop - &object_schema.persisted_properties[0]);
        if (!primary_value)
            primary_value = ctx.default_value_for_property(object_schema, *primary_prop);
<<<<<<< HEAD
        if (!primary_value) {
            if (!is_nullable(primary_prop->type))
                throw MissingPropertyValueException(object_schema.name, primary_prop->name);
            primary_value = ctx.null_value();
        }
        row_index = get_for_primary_key_impl(ctx, *table, *primary_prop, *primary_value);

        if (row_index == realm::not_found) {
            created = true;
            if (primary_prop->type == PropertyType::Int) {
#if REALM_ENABLE_SYNC
                row_index = sync::create_object_with_primary_key(realm->read_group(), *table, ctx.template unbox<util::Optional<int64_t>>(*primary_value));
#else
                row_index = table->add_empty_row();
                if (ctx.is_null(*primary_value))
                    table->set_null_unique(primary_prop->table_column, row_index);
                else
                    table->set_unique(primary_prop->table_column, row_index, ctx.template unbox<int64_t>(*primary_value));
#endif // REALM_ENABLE_SYNC
            }
            else if (primary_prop->type == PropertyType::String) {
                auto value = ctx.template unbox<StringData>(*primary_value);
#if REALM_ENABLE_SYNC
                row_index = sync::create_object_with_primary_key(realm->read_group(), *table, value);
#else
                row_index = table->add_empty_row();
                table->set_unique(primary_prop->table_column, row_index, value);
#endif // REALM_ENABLE_SYNC
            }
            else {
                REALM_TERMINATE("Unsupported primary key type.");
            }
        }
        else if (policy == CreatePolicy::ForceCreate) {
            if (realm->is_in_migration()) {
                // Creating objects with duplicate primary keys is allowed in migrations
                // as long as there are no duplicates at the end, as adding an entirely
                // new column which is the PK will inherently result in duplicates at first
                row_index = table->add_empty_row();
                created = true;
                skip_primary = false;
            }
            else {
                throw std::logic_error(util::format("Attempting to create an object of type '%1' with an existing primary key value '%2'.",
                                                    object_schema.name, ctx.print(*primary_value)));
            }
        }
    }
    else {
        if (policy == CreatePolicy::UpdateModified && current_row != realm::not_found) {
            row_index = current_row;
        }
        else {
#if REALM_ENABLE_SYNC
            row_index = sync::create_object(realm->read_group(), *table);
#else
            row_index = table->add_empty_row();
#endif // REALM_ENABLE_SYNC
=======
        if (!primary_value && !is_nullable(primary_prop->type))
            throw MissingPropertyValueException(object_schema.name, primary_prop->name);

        // When changing the primary key of a table, we remove the existing pk (if any), call
        // the migration function, then add the new pk (if any). This means that we can't call
        // create_object_with_primary_key(), and creating duplicate primary keys is allowed as
        // long as they're unique by the end of the migration.
        if (table->get_primary_key_column() == ColKey{}) {
            REALM_ASSERT(realm->is_in_migration());
            if (policy != CreatePolicy::ForceCreate) {
                if (auto key = get_for_primary_key_impl(ctx, *table, *primary_prop, *primary_value))
                    obj = table->get_object(key);
            }
            if (!obj)
                skip_primary = false;
        }
        else {
            obj = table->create_object_with_primary_key(as_mixed(ctx, primary_value, primary_prop->type), &created);
            if (!created && policy == CreatePolicy::ForceCreate) {
                if (!realm->is_in_migration()) {
                    throw std::logic_error(util::format("Attempting to create an object of type '%1' with an existing primary key value '%2'.",
                                                        object_schema.name, ctx.print(*primary_value)));
                }
                table->set_primary_key_column(ColKey{});
                skip_primary = false;
                obj = {};
            }
        }
    }

    // No primary key (possibly temporarily due to migrations). If we're
    // currently performing a recursive update on an existing object tree then
    // an object key was passed in that we need to look up, and otherwise we
    // need to create the new object.
    if (!obj) {
        if (policy == CreatePolicy::UpdateModified && current_obj) {
            obj = table->get_object(current_obj);
        }
        else {
            obj = table->create_object();
>>>>>>> origin/develop12
            created = true;
        }
    }

<<<<<<< HEAD
    // populate
    Object object(realm, object_schema, table->get(row_index));
    if (out_row)
        *out_row = object.row();
=======
    Object object(realm, object_schema, obj);
    // KVO in Cocoa requires that the obj ivar on the wrapper object be set
    // *before* we start setting the properties, so it passes in a pointer to
    // that.
    if (out_row)
        *out_row = obj;
>>>>>>> origin/develop12
    for (size_t i = 0; i < object_schema.persisted_properties.size(); ++i) {
        auto& prop = object_schema.persisted_properties[i];
        if (skip_primary && prop.is_primary)
            continue;

        auto v = ctx.value_for_property(value, prop, i);
        if (!created && !v)
            continue;

        bool is_default = false;
        if (!v) {
            v = ctx.default_value_for_property(object_schema, prop);
            is_default = true;
        }
<<<<<<< HEAD
=======
        // We consider null or a missing value to be equivalent to an empty
        // array for historical reasons; the original implementation did this
        // accidentally and it's not worth changing.
>>>>>>> origin/develop12
        if ((!v || ctx.is_null(*v)) && !is_nullable(prop.type) && !is_array(prop.type)) {
            if (prop.is_primary || !ctx.allow_missing(value))
                throw MissingPropertyValueException(object_schema.name, prop.name);
        }
        if (v)
            object.set_property_value_impl(ctx, prop, *v, policy, is_default);
    }
#if REALM_ENABLE_SYNC
    if (realm->is_partial() && object_schema.name == "__User") {
        object.ensure_user_in_everyone_role();
        object.ensure_private_role_exists_for_user();
    }
#endif
    return object;
}

template<typename ValueType, typename ContextType>
Object Object::get_for_primary_key(ContextType& ctx, std::shared_ptr<Realm> const& realm,
                      StringData object_type, ValueType primary_value)
{
    auto object_schema = realm->schema().find(object_type);
    REALM_ASSERT(object_schema != realm->schema().end());
    return get_for_primary_key(ctx, realm, *object_schema, primary_value);
}

template<typename ValueType, typename ContextType>
Object Object::get_for_primary_key(ContextType& ctx, std::shared_ptr<Realm> const& realm,
                                   const ObjectSchema &object_schema,
                                   ValueType primary_value)
{
    auto primary_prop = object_schema.primary_key_property();
    if (!primary_prop) {
        throw MissingPrimaryKeyException(object_schema.name);
    }

<<<<<<< HEAD
    auto table = ObjectStore::table_for_object_type(realm->read_group(), object_schema.name);
    if (!table)
        return Object(realm, object_schema, RowExpr());
    auto row_index = get_for_primary_key_impl(ctx, *table, *primary_prop, primary_value);

    return Object(realm, object_schema, row_index == realm::not_found ? Row() : Row(table->get(row_index)));
}

template<typename ValueType, typename ContextType>
size_t Object::get_for_primary_key_impl(ContextType& ctx, Table const& table,
=======
    TableRef table;
    if (object_schema.table_key)
        table = realm->read_group().get_table(object_schema.table_key);
    if (!table)
        return Object(realm, object_schema, Obj());
    auto key = get_for_primary_key_impl(ctx, *table, *primary_prop, primary_value);
    return Object(realm, object_schema, key ? table->get_object(key) : Obj{});
}

template<typename ValueType, typename ContextType>
ObjKey Object::get_for_primary_key_impl(ContextType& ctx, Table const& table,
>>>>>>> origin/develop12
                                        const Property &primary_prop,
                                        ValueType primary_value) {
    bool is_null = ctx.is_null(primary_value);
    if (is_null && !is_nullable(primary_prop.type))
        throw std::logic_error("Invalid null value for non-nullable primary key.");
    if (primary_prop.type == PropertyType::String) {
<<<<<<< HEAD
        return table.find_first(primary_prop.table_column,
                                ctx.template unbox<StringData>(primary_value));
    }
    if (is_nullable(primary_prop.type))
        return table.find_first(primary_prop.table_column,
                                ctx.template unbox<util::Optional<int64_t>>(primary_value));
    return table.find_first(primary_prop.table_column,
=======
        return table.find_first(primary_prop.column_key,
                                ctx.template unbox<StringData>(primary_value));
    }
    if (is_nullable(primary_prop.type))
        return table.find_first(primary_prop.column_key,
                                ctx.template unbox<util::Optional<int64_t>>(primary_value));
    return table.find_first(primary_prop.column_key,
>>>>>>> origin/develop12
                            ctx.template unbox<int64_t>(primary_value));
}

} // namespace realm

#endif // REALM_OS_OBJECT_ACCESSOR_HPP
