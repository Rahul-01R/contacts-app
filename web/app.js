// =========================================
// GET HTML ELEMENTS
// =========================================

const contactsContainer =
    document.getElementById("contacts-container");

const addContactButton =
    document.getElementById("addContact");

const searchInput =
    document.getElementById("search");

const contactDialog =
    document.getElementById("contactDialog");

const contactForm =
    document.getElementById("contactForm");

const dialogTitle =
    document.getElementById("dialogTitle");

const closeDialog =
    document.getElementById("closeDialog");

const cancelContact =
    document.getElementById("cancelContact");

const contactId =
    document.getElementById("contactId");

const nameInput =
    document.getElementById("name");

const phoneInput =
    document.getElementById("phone");

const emailInput =
    document.getElementById("email");

const addressInput =
    document.getElementById("address");

const messageElement =
    document.getElementById("app-message");


// =========================================
// DISPLAY MESSAGE
// =========================================

function showMessage(message, isError = false) {

    messageElement.textContent = message;

    messageElement.className = isError
        ? "error-message"
        : "success-message";

}


// =========================================
// GET INITIALS
// =========================================

function getInitials(name) {

    return name
        .trim()
        .split(/\s+/)
        .map(word => word[0])
        .join("")
        .substring(0, 2)
        .toUpperCase();

}


// =========================================
// GET CONTACT ARRAY FROM RESPONSE
// =========================================

function extractContacts(data) {

    if (Array.isArray(data)) {

        return data;

    }

    if (Array.isArray(data?.contacts)) {

        return data.contacts;

    }

    if (Array.isArray(data?.data)) {

        return data.data;

    }

    if (Array.isArray(data?.results)) {

        return data.results;

    }

    if (Array.isArray(data?.data?.contacts)) {

        return data.data.contacts;

    }

    return null;

}


// =========================================
// DISPLAY CONTACTS
// =========================================

function renderContacts(contacts) {

    contactsContainer.replaceChildren();


    if (contacts.length === 0) {

        const emptyMessage =
            document.createElement("p");

        emptyMessage.className = "empty-message";

        emptyMessage.textContent =
            searchInput.value.trim()
                ? "No matching contacts found."
                : "No contacts yet. Add your first one.";

        contactsContainer.appendChild(emptyMessage);

        return;

    }


    contacts.forEach(contact => {

        const card =
            document.createElement("div");

        card.className = "contact-card";


        // Initials

        const initials =
            document.createElement("div");

        initials.className = "contact-initials";

        initials.textContent =
            getInitials(contact.name || "Unknown");


        // Name

        const name =
            document.createElement("h3");

        name.textContent = contact.name || "";


        // Phone

        const phone =
            document.createElement("p");

        phone.textContent =
            "Phone: " + (contact.phone || "Not provided");


        // Email

        const email =
            document.createElement("p");

        email.textContent =
            "Email: " + (contact.email || "Not provided");


        // Address

        const address =
            document.createElement("p");

        address.textContent =
            "Address: " + (contact.address || "Not provided");


        // Edit button

        const editButton =
            document.createElement("button");

        editButton.textContent = "Edit";

        editButton.className = "edit-btn";

        editButton.addEventListener("click", () => {

            openEditDialog(contact);

        });


        // Delete button

        const deleteButton =
            document.createElement("button");

        deleteButton.textContent = "Delete";

        deleteButton.className = "delete-btn";

        deleteButton.addEventListener("click", () => {

            handleDelete(contact.id);

        });


        card.append(
            initials,
            name,
            phone,
            email,
            address,
            editButton,
            deleteButton
        );


        contactsContainer.appendChild(card);

    });

}


// =========================================
// LOAD CONTACTS
// =========================================

async function loadContacts() {

    showMessage("Loading contacts...");

    const result = await getContacts();


    if (!result.ok) {

        showMessage(
            result.data?.message || "Unable to load contacts.",
            true
        );

        contactsContainer.replaceChildren();

        return;

    }


    const contacts = extractContacts(result.data);


    if (contacts === null) {

        showMessage(
            "Unexpected response format from backend.",
            true
        );

        return;

    }


    showMessage("");

    renderContacts(contacts);

}


// =========================================
// SEARCH CONTACTS
// =========================================

let searchTimeout = null;


searchInput.addEventListener("input", () => {

    clearTimeout(searchTimeout);


    searchTimeout = setTimeout(async () => {

        const query = searchInput.value.trim();


        if (query === "") {

            await loadContacts();

            return;

        }


        showMessage("Searching...");

        const result = await searchContacts(query);


        if (!result.ok) {

            showMessage(
                result.data?.message || "Search failed.",
                true
            );

            return;

        }


        const contacts = extractContacts(result.data);


        if (contacts === null) {

            showMessage(
                "Unexpected search response format.",
                true
            );

            return;

        }


        showMessage("");

        renderContacts(contacts);

    }, 300);

});


// =========================================
// OPEN ADD CONTACT DIALOG
// =========================================

addContactButton.addEventListener("click", () => {

    contactForm.reset();

    contactId.value = "";

    dialogTitle.textContent = "Add Contact";

    contactDialog.showModal();

});


// =========================================
// OPEN EDIT CONTACT DIALOG
// =========================================

function openEditDialog(contact) {

    contactId.value = contact.id;

    nameInput.value = contact.name || "";

    phoneInput.value = contact.phone || "";

    emailInput.value = contact.email || "";

    addressInput.value = contact.address || "";

    dialogTitle.textContent = "Edit Contact";

    contactDialog.showModal();

}


// =========================================
// CLOSE DIALOG
// =========================================

closeDialog.addEventListener("click", () => {

    contactDialog.close();

});


cancelContact.addEventListener("click", () => {

    contactDialog.close();

});


// =========================================
// ADD / UPDATE CONTACT
// =========================================

contactForm.addEventListener("submit", async (event) => {

    event.preventDefault();


    const contactData = {

        name: nameInput.value.trim(),

        phone: phoneInput.value.trim(),

        email: emailInput.value.trim(),

        address: addressInput.value.trim()

    };


    // Validation

    if (!contactData.name) {

        showMessage("Name is required.", true);

        return;

    }


    const digits =
        contactData.phone.replace(/\D/g, "");


    if (digits.length < 10) {

        showMessage(
            "Phone number must contain at least 10 digits.",
            true
        );

        return;

    }


    const id = contactId.value;

    let result;


    // Update existing contact

    if (id) {

        result = await updateContact(id, contactData);

    }

    // Create new contact

    else {

        result = await createContact(contactData);

    }


    // Handle errors

    if (!result.ok) {

        if (result.status === 409) {

            showMessage(
                "Duplicate phone number! This contact already exists.",
                true
            );

        }

        else if (result.status === 400) {

            showMessage(
                result.data?.message || "Invalid contact details.",
                true
            );

        }

        else if (result.status === 404) {

            showMessage(
                "Contact not found.",
                true
            );

        }

        else {

            showMessage(
                result.data?.message || "Unable to save contact.",
                true
            );

        }

        return;

    }


    // Success

    contactDialog.close();

    contactForm.reset();

    showMessage(
        id
            ? "Contact updated successfully!"
            : "Contact added successfully!"
    );


    await loadContacts();

});


// =========================================
// DELETE CONTACT
// =========================================

async function handleDelete(id) {

    if (id === undefined || id === null) {

        showMessage(
            "Contact ID is missing.",
            true
        );

        return;

    }


    const confirmed = confirm(
        "Are you sure you want to delete this contact?"
    );


    if (!confirmed) {

        return;

    }


    const result = await deleteContact(id);


    if (!result.ok) {

        showMessage(
            result.data?.message || "Unable to delete contact.",
            true
        );

        return;

    }


    showMessage("Contact deleted successfully!");

    await loadContacts();

}


// =========================================
// INITIAL APPLICATION LOAD
// =========================================

async function initializeApp() {

    const server = await checkServer();


    if (!server.ok) {

        showMessage(
            "Backend server is not responding. Start the C++ server.",
            true
        );

        return;

    }


    await loadContacts();

}


initializeApp();