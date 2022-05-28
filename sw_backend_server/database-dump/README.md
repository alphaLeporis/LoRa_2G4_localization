# Restore Database

During development and testing I made use of Supabase: [https://supabase.com/](https://supabase.com/)

To restore the database layout you can use:

- Jetbrains DataGrip to upload the schema to your Supabase instance (hosted)
- Make use of the SQL commands that are inside the *database_dump.sql* file.
- The following code can be used to set the schema to your own PostgreSQL database:

```bash
createdb -T template0 dbname
psql dbname < database_dump.sql
```

May 2022
– Louis de Looze