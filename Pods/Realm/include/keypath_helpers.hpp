////////////////////////////////////////////////////////////////////////////
//
// Copyright 2017 Realm Inc.
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
#include "object_store.hpp"
#include "shared_realm.hpp"

#include <realm/parser/keypath_mapping.hpp>

namespace realm {
/// Populate the mapping from public name to internal name for queries.
static void populate_keypath_mapping(parser::KeyPathMapping& mapping, Realm& realm)
{
    mapping.set_backlink_class_prefix("class_");

    for (auto& object_schema : realm.schema()) {
        TableRef table;
        auto get_table = [&] {
            if (!table)
<<<<<<< HEAD
                table = ObjectStore::table_for_object_type(realm.read_group(), object_schema.name);
=======
                table = realm.read_group().get_table(object_schema.table_key);
>>>>>>> origin/develop12
            return table;
        };

        for (auto& property : object_schema.persisted_properties) {
            if (!property.public_name.empty() && property.public_name != property.name)
                mapping.add_mapping(get_table(), property.public_name, property.name);
        }

        for (auto& property : object_schema.computed_properties) {
            if (property.type != PropertyType::LinkingObjects)
                continue;
            auto native_name = util::format("@links.%1.%2", property.object_type,
                                            property.link_origin_property_name);
            mapping.add_mapping(get_table(), property.name, std::move(native_name));
        }
    }
}

/// Generate an IncludeDescriptor from a list of key paths.
///
/// Each key path in the list is a period ('.') seperated property path, beginning
/// at the class defined by `object_schema` and ending with a linkingObjects relationship.
inline IncludeDescriptor generate_include_from_keypaths(std::vector<StringData> const& paths,
                                                        Realm& realm, ObjectSchema const& object_schema,
                                                        parser::KeyPathMapping& mapping)
{
<<<<<<< HEAD
    auto base_table = ObjectStore::table_for_object_type(realm.read_group(), object_schema.name);
=======
    auto base_table = realm.read_group().get_table(object_schema.table_key);
>>>>>>> origin/develop12
    REALM_ASSERT(base_table);
    // FIXME: the following is mostly copied from core's query_builder::apply_ordering
    std::vector<std::vector<LinkPathPart>> properties;
    for (size_t i = 0; i < paths.size(); ++i) {
        StringData keypath = paths[i];
        if (keypath.size() == 0) {
            throw InvalidPathError("missing property name while generating INCLUDE from keypaths");
        }

        util::KeyPath path = util::key_path_from_string(keypath);
        size_t index = 0;
        std::vector<LinkPathPart> links;
        ConstTableRef cur_table = base_table;

        while (index < path.size()) {
            parser::KeyPathElement element = mapping.process_next_path(cur_table, path, index); // throws if invalid
            // backlinks use type_LinkList since list operations apply to them (and is_backlink is set)
            if (element.col_type != type_Link && element.col_type != type_LinkList) {
                throw InvalidPathError(util::format("Property '%1' is not a link in object of type '%2' in 'INCLUDE' clause",
<<<<<<< HEAD
                                                    element.table->get_column_name(element.col_ndx),
                                                    get_printable_table_name(*element.table)));
            }
            if (element.table == cur_table) {
                if (element.col_ndx == realm::npos) {
                    cur_table = element.table;
                }
                else {
                    cur_table = element.table->get_link_target(element.col_ndx); // advance through forward link
=======
                                                    element.table->get_column_name(element.col_key),
                                                    get_printable_table_name(*element.table)));
            }
            if (element.table == cur_table) {
                if (!element.col_key) {
                    cur_table = element.table;
                }
                else {
                    cur_table = element.table->get_link_target(element.col_key); // advance through forward link
>>>>>>> origin/develop12
                }
            }
            else {
                cur_table = element.table; // advance through backlink
            }
<<<<<<< HEAD
            ConstTableRef tr;
            if (element.is_backlink) {
                tr = element.table;
            }
            links.emplace_back(element.col_ndx, tr);
        }
        properties.push_back(std::move(links));
    }
    return IncludeDescriptor{*base_table, properties};
=======
            LinkPathPart link = element.is_backlink ? LinkPathPart(element.col_key, element.table) : LinkPathPart(element.col_key);
            links.emplace_back(std::move(link));
        }
        properties.push_back(std::move(links));
    }
    return IncludeDescriptor{base_table, properties};
>>>>>>> origin/develop12
}
}
