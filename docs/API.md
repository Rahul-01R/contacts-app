# API Contract

## The Contact
* *id:* number (set by the server, client never sends it on create)
* *name:* string (required, trimmed, 1 to 100 characters)
* *phone:* string (required, 10 to 15 digits once symbols removed)
* *email:* string (optional, empty or valid format, max 100 characters)
* *address:* string (optional, max 300 characters)

## Endpoints

### GET /api/contacts
* *Request:* none
* *Success (200):* array of contacts sorted by name
* *Errors:* none

### POST /api/contacts
* *Request:* JSON object (name, phone, email, address)
* *Success (201):* object with contact and warnings
* *Errors:* 400 validation or bad_json, 409 duplicate_phone
{
  "contact": {
    "address": "12 MG Road, Pune",
    "email": "asha@example.com",
    "id": 1,
    "name": "Asha Verma",
    "phone": "+91 98765 43210"
  },
  "warnings": []
}
{
  "error": "duplicate_phone",
  "existing": {
    "address": "12 MG Road, Pune",
    "email": "asha@example.com",
    "id": 1,
    "name": "Asha Verma",
    "phone": "+91 98765 43210"
  },
  "message": "This number already belongs to Asha Verma"
}
{
  "error": "validation",
  "field": "name",
  "message": "Name is required"
}
### PUT /api/contacts/{id}
* *Request:* same JSON as POST
* *Success (200):* object with contact and warnings
* *Errors:* 400, 404 not_found, 409 duplicate_phone
{
  "contact": {
    "address": "12 MG Road, Pune",
    "email": "asha@example.com",
    "id": 1,
    "name": "Asha Verma",
    "phone": "+91 98765 43210"
  },
  "warnings": []
}
### DELETE /api/contacts/{id}
* *Request:* none
* *Success (200):* object with deleted (the id)
* *Errors:* 404 not_found
*
{
  "deleted": 1
}