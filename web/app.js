// ========================================
// 1. GET HTML ELEMENTS
// ========================================

const contactsContainer = document.getElementById("contacts-container");
const addContactButton = document.getElementById("addContact");
const searchInput = document.getElementById("search");
const contactDialog = document.getElementById("contactDialog");
const contactForm = document.getElementById("contactForm");
const dialogTitle = document.getElementById("dialogTitle");
const closeDialogButton = document.getElementById("closeDialog");
const cancelContactButton = document.getElementById("cancelContact");
const contactIdInput = document.getElementById("contactId");
const nameInput = document.getElementById("name");
const phoneInput = document.getElementById("phone");
const emailInput = document.getElementById("email");
const addressInput = document.getElementById("address");
const saveContactButton = document.getElementById("saveContact");


// ========================================
// 2. LOAD CONTACTS
// ========================================

let allContacts = [];

async function loadContacts() {
    try {
        const response = await getContacts();

        // Check whether the API operation succeeded
        if (!response.ok) {
            throw new Error(
                response.data?.message || "Unable to load contacts."
            );
        }

        // Store contacts returned by the API
        allContacts = Array.isArray(response.data)
            ? response.data
            : [];

        // Display contacts using the current search query
        displayFilteredContacts();

    } catch (error) {
        console.error("Error loading contacts:", error);

        contactsContainer.replaceChildren();

        const message = document.createElement("p");
        message.className = "empty-message";
        message.textContent = "Unable to load contacts.";

        contactsContainer.appendChild(message);

        showMessage(
            error.message || "Unable to load contacts. Please try again.",
            "error"
        );
    }
}


// ========================================
// 3. DISPLAY SUCCESS AND ERROR MESSAGES
// ========================================

function showMessage(message, type = "success") {
    let messageBox = document.getElementById("appMessage");

    if (!messageBox) {
        messageBox = document.createElement("p");
        messageBox.id = "appMessage";
        messageBox.setAttribute("role", "status");
        messageBox.setAttribute("aria-live", "polite");

        contactsContainer.parentElement.prepend(messageBox);
    }

    messageBox.textContent = message;
    messageBox.className = `app-message ${type}`;
    messageBox.style.display = "block";
}

function clearMessage() {
    const messageBox = document.getElementById("appMessage");

    if (messageBox) {
        messageBox.textContent = "";
        messageBox.style.display = "none";
    }
}


// ========================================
// 4. RENDER CONTACT CARDS
// ========================================

function renderContacts(list) {
    contactsContainer.replaceChildren();

    // Handle empty lists and empty search results
    if (list.length === 0) {
        const message = document.createElement("p");

        message.className = "empty-message";

        if (allContacts.length === 0) {
            message.textContent =
                "No contacts yet. Add your first one.";
        } else {
            message.textContent =
                "No matching contacts found.";
        }

        contactsContainer.appendChild(message);

        return;
    }

    list.forEach(contact => {

        // Main card
        const card = document.createElement("article");
        card.className = "contact-card";

        // Contact header
        const top = document.createElement("div");
        top.className = "contact-top";

        // Initials avatar
        const avatar = document.createElement("div");
        avatar.className = "contact-avatar";

        const initials = String(contact.name || "")
            .trim()
            .split(/\s+/)
            .filter(Boolean)
            .slice(0, 2)
            .map(word => word[0].toUpperCase())
            .join("");

        avatar.textContent = initials || "?";

        // Contact name
        const heading = document.createElement("h3");
        heading.textContent = contact.name || "Unnamed Contact";

        const nameContainer = document.createElement("div");
        nameContainer.appendChild(heading);

        top.appendChild(avatar);
        top.appendChild(nameContainer);

        card.appendChild(top);

        // Phone number
        const phone = document.createElement("p");
        phone.textContent = "Phone: " + (contact.phone || "Not provided");
        card.appendChild(phone);

        // Email address
        const email = document.createElement("p");
        email.textContent =
            "Email: " + (contact.email || "Not provided");
        card.appendChild(email);

        // Physical address
        const address = document.createElement("p");
        address.textContent =
            "Address: " + (contact.address || "Not provided");
        card.appendChild(address);

        // Action buttons
        const actions = document.createElement("div");
        actions.className = "contact-actions";

        // Edit button
        const editButton = document.createElement("button");
        editButton.type = "button";
        editButton.className = "edit-button";
        editButton.textContent = "Edit";

        editButton.addEventListener("click", () => {
            openEditDialog(contact);
        });

        // Delete button
        const deleteButton = document.createElement("button");
        deleteButton.type = "button";
        deleteButton.className = "delete-button";
        deleteButton.textContent = "Delete";

        deleteButton.addEventListener("click", () => {
            handleDeleteContact(contact.id);
        });

        actions.appendChild(editButton);
        actions.appendChild(deleteButton);

        card.appendChild(actions);

        // Add card to the page
        contactsContainer.appendChild(card);
    });
}


// ========================================
// 5. SEARCH CONTACTS
// ========================================

function displayFilteredContacts() {
    const query = searchInput.value.trim().toLowerCase();

    const filteredContacts = allContacts.filter(contact => {
        const name = String(contact.name || "").toLowerCase();
        const phone = String(contact.phone || "").toLowerCase();
        const email = String(contact.email || "").toLowerCase();
        const address = String(contact.address || "").toLowerCase();

        return (
            name.includes(query) ||
            phone.includes(query) ||
            email.includes(query) ||
            address.includes(query)
        );
    });

    renderContacts(filteredContacts);
}

searchInput.addEventListener("input", displayFilteredContacts);


// ========================================
// 6. CLEAR VALIDATION ERRORS
// ========================================

function clearErrors() {
    const fields = [
        nameInput,
        phoneInput,
        emailInput,
        addressInput
    ];

    const errorIds = [
        "nameError",
        "phoneError",
        "emailError",
        "addressError"
    ];

    fields.forEach(field => {
        field.classList.remove("invalid");
        field.removeAttribute("aria-invalid");
    });

    errorIds.forEach(id => {
        const errorElement = document.getElementById(id);

        if (errorElement) {
            errorElement.textContent = "";
        }
    });
}


// ========================================
// 7. DISPLAY A FIELD ERROR
// ========================================

function showError(field, errorId, message) {
    field.classList.add("invalid");
    field.setAttribute("aria-invalid", "true");

    const errorElement = document.getElementById(errorId);

    if (errorElement) {
        errorElement.textContent = message;
    }
}


// ========================================
// 8. VALIDATE THE FORM
// ========================================

function validateForm() {
    clearErrors();

    let isValid = true;

    const name = nameInput.value.trim();
    const phone = phoneInput.value.trim();
    const email = emailInput.value.trim();

    // Validate name
    if (name === "") {
        showError(
            nameInput,
            "nameError",
            "Name must not be empty."
        );

        isValid = false;
    }

    // Validate phone number
    const digits = phone.replace(/\D/g, "");

    if (digits.length < 10) {
        showError(
            phoneInput,
            "phoneError",
            "Phone must contain at least 10 digits."
        );

        isValid = false;
    }

    // Validate email only when supplied
    if (
        email !== "" &&
        !/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email)
    ) {
        showError(
            emailInput,
            "emailError",
            "Enter a valid email address."
        );

        isValid = false;
    }

    return isValid;
}


// ========================================
// 9. OPEN THE ADD CONTACT DIALOG
// ========================================

function openAddDialog() {
    clearMessage();

    contactForm.reset();
    clearErrors();

    contactIdInput.value = "";

    dialogTitle.textContent = "Add Contact";
    saveContactButton.textContent = "Save Contact";

    contactDialog.showModal();

    nameInput.focus();
}

addContactButton.addEventListener("click", openAddDialog);


// ========================================
// 10. OPEN THE EDIT CONTACT DIALOG
// ========================================

function openEditDialog(contact) {
    clearMessage();

    contactForm.reset();
    clearErrors();

    // Store the selected contact ID
    contactIdInput.value = contact.id;

    // Fill in the existing details
    nameInput.value = contact.name || "";
    phoneInput.value = contact.phone || "";
    emailInput.value = contact.email || "";
    addressInput.value = contact.address || "";

    // Update dialog title and button
    dialogTitle.textContent = "Edit Contact";
    saveContactButton.textContent = "Update Contact";

    // Open the dialog
    contactDialog.showModal();

    nameInput.focus();
}


// ========================================
// 11. CLOSE THE DIALOG
// ========================================

function closeContactDialog() {
    if (contactDialog.open) {
        contactDialog.close();
    }

    contactForm.reset();
    clearErrors();

    contactIdInput.value = "";
}

closeDialogButton.addEventListener(
    "click",
    closeContactDialog
);

cancelContactButton.addEventListener(
    "click",
    closeContactDialog
);


// ========================================
// 12. ADD OR UPDATE A CONTACT
// ========================================

contactForm.addEventListener("submit", async function (event) {
    event.preventDefault();

    clearMessage();

    // Validate the form
    if (!validateForm()) {
        return;
    }

    // Collect contact details
    const contactData = {
        name: nameInput.value.trim(),
        phone: phoneInput.value.trim(),
        email: emailInput.value.trim(),
        address: addressInput.value.trim()
    };

    // Determine whether we are adding or editing
    const contactId = contactIdInput.value;

    // Prevent repeated submissions
    saveContactButton.disabled = true;

    try {
        let response;

        if (contactId === "") {
            // Create a new contact
            response = await createContact(contactData);
        } else {
            // Update an existing contact
            response = await updateContact(
                contactId,
                contactData
            );
        }

        // Handle unsuccessful API responses
        if (!response.ok) {
            showMessage(
                response.data?.message ||
                "Unable to save the contact. Please try again.",
                "error"
            );

            return;
        }

        // Close dialog after successful saving
        closeContactDialog();

        // Reload contacts
        await loadContacts();

        // Display success message
        if (contactId === "") {
            showMessage(
                "Contact added successfully.",
                "success"
            );
        } else {
            showMessage(
                "Contact updated successfully.",
                "success"
            );
        }

    } catch (error) {
        console.error("Error saving contact:", error);

        showMessage(
            error.message ||
            "Unable to save the contact. Please try again.",
            "error"
        );

    } finally {
        saveContactButton.disabled = false;
    }
});


// ========================================
// 13. DELETE A CONTACT
// ========================================

async function handleDeleteContact(id) {
    // Ask for confirmation
    const confirmed = window.confirm(
        "Are you sure you want to delete this contact?"
    );

    if (!confirmed) {
        return;
    }

    clearMessage();

    try {
        // Delete the selected contact
        const response = await deleteContact(id);

        // Handle unsuccessful API responses
        if (!response.ok) {
            showMessage(
                response.data?.message ||
                "Unable to delete the contact.",
                "error"
            );

            return;
        }

        // Reload and display updated contacts
        await loadContacts();

        showMessage(
            "Contact deleted successfully.",
            "success"
        );

    } catch (error) {
        console.error("Error deleting contact:", error);

        showMessage(
            error.message ||
            "Unable to delete the contact. Please try again.",
            "error"
        );
    }
}


// ========================================
// 14. START THE APPLICATION
// ========================================

loadContacts();